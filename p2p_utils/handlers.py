# p2p_utils/handlers.py
import socket
from p2p_utils.classes import Neighbor
from p2p_utils.utils import safe_close, send_line


def register_peer(state):
    try:
        state.udp_sock.settimeout(3.0)
        state.udp_sock.sendto("REG {}\n".format(state.lnkport).encode("ascii"), (state.server_addr, state.server_port))
        data, _ = state.udp_sock.recvfrom(1024)
    except OSError as exc:
        print("Registration failed: {}".format(exc))
        return None
    finally:
        state.udp_sock.settimeout(None)
    parts = data.decode("ascii", errors="ignore").strip().split()
    if len(parts) == 2 and parts[0] == "SQN":
        try:
            return int(parts[1])
        except ValueError:
            return None
    print("Registration failed")
    return None


def unregister_peer(state):
    if state.seqnumber is None:
        return
    try:
        state.udp_sock.settimeout(3.0)
        state.udp_sock.sendto("UNR {}\n".format(state.seqnumber).encode("ascii"), (state.server_addr, state.server_port))
        state.udp_sock.recvfrom(1024)
    except OSError:
        pass
    finally:
        state.udp_sock.settimeout(None)


def get_peers(state):
    peers = []
    try:
        state.udp_sock.settimeout(3.0)
        state.udp_sock.sendto(b"PEERS\n", (state.server_addr, state.server_port))
        data, _ = state.udp_sock.recvfrom(65535)
    except OSError as exc:
        print("Failed to retrieve peers: {}".format(exc))
        return peers
    finally:
        state.udp_sock.settimeout(None)

    lines = data.decode("ascii", errors="ignore").splitlines()
    if not lines or lines[0].strip() != "LST":
        return peers
    for line in lines[1:]:
        line = line.strip()
        if not line:
            break
        try:
            addr_part, seq_part = line.rsplit("#", 1)
            ip_part, port_part = addr_part.split(":")
            seq = int(seq_part)
            port = int(port_part)
            peers.append((ip_part, port, seq))
        except ValueError:
            continue
    state.known_peers = {seq: (ip, port) for ip, port, seq in peers}
    return peers


def connect_to_peer(state, ip, port, seq, force=False):
    if seq == state.seqnumber or len(state.neighbors_out) >= state.neigh:
        return False
    sock = None
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(3.0)
        sock.connect((ip, port))
        cmd = "FRC" if force else "LNK"
        sock.sendall("{} {}\n".format(cmd, state.seqnumber).encode("ascii"))
        data = sock.recv(1024)
        if not data:
            safe_close(sock)
            return False
        line = data.decode("ascii", errors="ignore").strip()
        if line != "CNF":
            safe_close(sock)
            return False
        sock.settimeout(None)
        sock.setblocking(False)
        neighbor = Neighbor(sock, ip, port, seq, "out")
        state.neighbors_out[seq] = neighbor
        state.sock_buffers[sock] = ""
        state.sock_to_seq[sock] = seq
        return True
    except OSError:
        safe_close(sock)
        return False


def connect_to_candidates(state, peers, allow_force=False):
    used_force = False
    for ip, port, seq in sorted(peers, key=lambda x: x[2]):
        if seq >= (state.seqnumber or 0):
            continue
        if seq in state.neighbors_in or seq in state.neighbors_out:
            continue
        if len(state.neighbors_out) >= state.neigh:
            break
        force = allow_force and not used_force
        success = connect_to_peer(state, ip, port, seq, force=force)
        if success and force:
            used_force = True


def accept_connection(state):
    try:
        conn, addr = state.tcp_listen.accept()
        conn.setblocking(False)
        state.pending_handshakes[conn] = {"addr": addr, "buffer": ""}
    except BlockingIOError:
        return
    except OSError:
        return


def process_handshake(state, sock):
    info = state.pending_handshakes.get(sock)
    if info is None:
        return
    buffer = info["buffer"]
    try:
        data = sock.recv(4096)
    except OSError:
        close_socket(state, sock)
        return
    if not data:
        close_socket(state, sock)
        return
    buffer += data.decode("ascii", errors="ignore")
    if "\n" not in buffer:
        info["buffer"] = buffer
        return
    line, remainder = buffer.split("\n", 1)
    del state.pending_handshakes[sock]
    parts = line.strip().split()
    if len(parts) != 2 or parts[0] not in ("LNK", "FRC"):
        safe_close(sock)
        return
    try:
        remote_seq = int(parts[1])
    except ValueError:
        safe_close(sock)
        return
    addr = sock.getpeername()
    ip = addr[0]
    lnkport = state.known_peers.get(remote_seq, (ip, addr[1]))[1]
    accepted = False
    if parts[0] == "LNK":
        if len(state.neighbors_in) < state.neigh:
            accepted = True
    else:
        if len(state.neighbors_in) < state.neigh:
            accepted = True
        else:
            higher = [seq for seq in state.neighbors_in if seq > remote_seq]
            if higher:
                drop_seq = max(higher)
                remove_neighbor_by_seq(state, drop_seq, direction="in")
                accepted = True
    if not accepted:
        safe_close(sock)
        return
    try:
        send_line(sock, "CNF")
    except OSError:
        safe_close(sock)
        return
    sock.setblocking(False)
    neighbor = Neighbor(sock, ip, lnkport, remote_seq, "in")
    state.neighbors_in[remote_seq] = neighbor
    state.sock_buffers[sock] = remainder
    state.sock_to_seq[sock] = remote_seq
    if remainder:
        process_buffer(state, sock)


def handle_neighbor_socket(state, sock):
    try:
        data = sock.recv(4096)
    except OSError:
        close_socket(state, sock)
        return
    if not data:
        close_socket(state, sock)
        return
    buffer = state.sock_buffers.get(sock, "") + data.decode("ascii", errors="ignore")
    state.sock_buffers[sock] = buffer
    process_buffer(state, sock)


def process_buffer(state, sock):
    buffer = state.sock_buffers.get(sock, "")
    if "\n" not in buffer:
        state.sock_buffers[sock] = buffer
        return
    lines = buffer.split("\n")
    state.sock_buffers[sock] = lines[-1]
    for line in lines[:-1]:
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        if not parts:
            continue
        cmd = parts[0]
        if cmd == "QRY" and len(parts) == 3:
            try:
                hopcount = int(parts[2])
            except ValueError:
                continue
            handle_qry(state, sock, parts[1], hopcount)
        elif cmd == "FND" and len(parts) == 2:
            handle_fnd(state, sock, parts[1])
        elif cmd == "NOTFND" and len(parts) == 2:
            handle_notfnd(state, sock, parts[1])


def handle_qry(state, origin_sock, identifier, hopcount):
    if identifier in state.identifiers:
        try:
            send_line(origin_sock, "FND {}".format(identifier))
        except OSError:
            close_socket(state, origin_sock)
        return
    if hopcount <= 1:
        try:
            send_line(origin_sock, "NOTFND {}".format(identifier))
        except OSError:
            close_socket(state, origin_sock)
        return
    targets = []
    for neighbor in list(state.neighbors_in.values()) + list(state.neighbors_out.values()):
        if neighbor.sock is not origin_sock:
            targets.append(neighbor.sock)
    if not targets:
        try:
            send_line(origin_sock, "NOTFND {}".format(identifier))
        except OSError:
            close_socket(state, origin_sock)
        return
    for sock in targets:
        try:
            send_line(sock, "QRY {} {}".format(identifier, hopcount - 1))
        except OSError:
            close_socket(state, sock)
    state.forwarded_queries[(identifier, origin_sock)] = {
        "pending": set(targets),
        "origin": origin_sock,
        "responded": False,
    }


def handle_fnd(state, sock, identifier):
    if identifier in state.local_queries:
        state.local_queries.pop(identifier)
        if identifier not in state.identifiers:
            state.identifiers.add(identifier)
            print("Identifier '{}' found".format(identifier))
        return
    for key, info in list(state.forwarded_queries.items()):
        if key[0] == identifier and sock in info["pending"]:
            if not info["responded"]:
                try:
                    send_line(info["origin"], "FND {}".format(identifier))
                except OSError:
                    close_socket(state, info["origin"])
            state.forwarded_queries.pop(key, None)
            break
    state.identifiers.add(identifier)


def handle_notfnd(state, sock, identifier):
    if identifier in state.local_queries:
        info = state.local_queries[identifier]
        info["pending"].discard(sock)
        if not info["pending"]:
            if not info.get("found"):
                print("Identifier '{}' not found".format(identifier))
            state.local_queries.pop(identifier, None)
        return
    for key, info in list(state.forwarded_queries.items()):
        if key[0] == identifier and sock in info["pending"]:
            info["pending"].discard(sock)
            if not info["pending"] and not info["responded"]:
                try:
                    send_line(info["origin"], "NOTFND {}".format(identifier))
                except OSError:
                    close_socket(state, info["origin"])
                state.forwarded_queries.pop(key, None)
            break


def close_socket(state, sock):
    state.sock_buffers.pop(sock, None)
    state.pending_handshakes.pop(sock, None)
    seq = state.sock_to_seq.pop(sock, None)
    removed_direction = None
    for mapping in (state.neighbors_in, state.neighbors_out):
        for key, neighbor in list(mapping.items()):
            if neighbor.sock is sock:
                removed_direction = neighbor.direction
                del mapping[key]
                break
    safe_close(sock)
    cleanup_queries_on_loss(state, sock)
    if removed_direction == "out":
        handle_external_loss(state)


def cleanup_queries_on_loss(state, sock):
    for identifier, info in list(state.local_queries.items()):
        if sock in info.get("pending", set()):
            info["pending"].discard(sock)
            if not info["pending"] and not info.get("found"):
                print("Identifier '{}' not found".format(identifier))
                state.local_queries.pop(identifier, None)
    for key, info in list(state.forwarded_queries.items()):
        if sock in info["pending"]:
            info["pending"].discard(sock)
            if not info["pending"] and not info["responded"]:
                try:
                    send_line(info["origin"], "NOTFND {}".format(key[0]))
                except OSError:
                    close_socket(state, info["origin"])
                state.forwarded_queries.pop(key, None)


def remove_neighbor_by_seq(state, seq, direction=None):
    if direction == "in":
        mapping = state.neighbors_in
    elif direction == "out":
        mapping = state.neighbors_out
    else:
        mapping = None
    mappings = [mapping] if mapping is not None else [state.neighbors_in, state.neighbors_out]
    for table in mappings:
        for key, neighbor in list(table.items()):
            if neighbor.seq == seq:
                close_socket(state, neighbor.sock)
                return True
    return False


def handle_external_loss(state):
    if not state.joined:
        return
    if state.neighbors_out:
        return
    peers = get_peers(state)
    connect_to_candidates(state, peers, allow_force=True)
    if not state.neighbors_out and state.neighbors_in:
        print("Warning: no external neighbors available")


def drop_all_neighbors(state):
    for neighbor in list(state.neighbors_in.values()) + list(state.neighbors_out.values()):
        safe_close(neighbor.sock)
    for sock in list(state.sock_buffers.keys()):
        safe_close(sock)
    state.neighbors_in.clear()
    state.neighbors_out.clear()
    state.sock_buffers.clear()
    state.sock_to_seq.clear()
    state.pending_handshakes.clear()
    state.local_queries.clear()
    state.forwarded_queries.clear()
