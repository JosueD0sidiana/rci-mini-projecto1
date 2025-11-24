# p2pnet.py
import argparse
import select
import sys

from p2p_utils.classes import PeerState
from p2p_utils import handlers
from p2p_utils.utils import send_line


def command_join(state):
    if state.joined:
        print("Already joined")
        return
    seq = handlers.register_peer(state)
    if seq is None:
        print("Failed to join network")
        return
    state.seqnumber = seq
    state.joined = True
    peers = handlers.get_peers(state)
    peers = [p for p in peers if p[2] != state.seqnumber]
    handlers.connect_to_candidates(state, peers, allow_force=False)
    if not state.neighbors_out:
        handlers.connect_to_candidates(state, peers, allow_force=True)
    if state.neighbors_out:
        print("Joined network")
    else:
        print("Joined network without external neighbors")


def command_leave(state):
    if not state.joined:
        print("Not in the network")
        return
    handlers.unregister_peer(state)
    handlers.drop_all_neighbors(state)
    state.joined = False
    state.seqnumber = None
    print("Left network")


def command_show_neighbors(state):
    print("Internal neighbors:")
    for neighbor in state.neighbors_in.values():
        print("  seq={} ip={} port={}".format(neighbor.seq, neighbor.ip, neighbor.port))
    print("External neighbors:")
    for neighbor in state.neighbors_out.values():
        print("  seq={} ip={} port={}".format(neighbor.seq, neighbor.ip, neighbor.port))


def command_release(state, seq):
    removed = handlers.remove_neighbor_by_seq(state, seq, direction="in")
    if not removed:
        print("No such internal neighbor")
    else:
        handlers.handle_external_loss(state)


def command_list_identifiers(state):
    for identifier in sorted(state.identifiers):
        print(identifier)


def command_post(state, identifier):
    state.identifiers.add(identifier)
    print("Posted '{}'".format(identifier))


def command_unpost(state, identifier):
    if identifier in state.identifiers:
        state.identifiers.remove(identifier)
        print("Unposted '{}'".format(identifier))
    else:
        print("Identifier not present")


def command_search(state, identifier):
    if not state.joined:
        print("Not joined")
        return
    if identifier in state.identifiers:
        print("Identifier already known")
        return
    targets = [n.sock for n in state.neighbors_in.values()] + [n.sock for n in state.neighbors_out.values()]
    if not targets:
        print("No neighbors to query")
        return
    for sock in targets:
        try:
            send_line(sock, "QRY {} {}".format(identifier, state.hc))
        except OSError:
            handlers.close_socket(state, sock)
    state.local_queries[identifier] = {"pending": set(targets), "found": False}
    print("Searching '{}'".format(identifier))


def handle_command(state, line):
    if not line:
        return
    parts = line.strip().split()
    if not parts:
        return
    cmd = parts[0].lower()
    if cmd == "join":
        command_join(state)
    elif cmd == "leave":
        command_leave(state)
    elif cmd == "show" and len(parts) >= 2 and parts[1].lower() == "neighbors":
        command_show_neighbors(state)
    elif cmd == "release" and len(parts) == 2:
        try:
            seq = int(parts[1])
        except ValueError:
            print("Invalid sequence number")
            return
        command_release(state, seq)
    elif cmd == "list" and len(parts) >= 2 and parts[1].lower() == "identifiers":
        command_list_identifiers(state)
    elif cmd == "post" and len(parts) == 2:
        command_post(state, parts[1])
    elif cmd == "unpost" and len(parts) == 2:
        command_unpost(state, parts[1])
    elif cmd == "search" and len(parts) == 2:
        command_search(state, parts[1])
    elif cmd == "exit":
        command_leave(state)
        sys.exit(0)
    else:
        print("Unknown command")


def run(state):
    while True:
        read_fds = [state.tcp_listen] + list(state.pending_handshakes.keys())
        read_fds += [n.sock for n in state.neighbors_in.values()]
        read_fds += [n.sock for n in state.neighbors_out.values()]
        read_fds.append(sys.stdin)
        try:
            readable, _, _ = select.select(read_fds, [], [])
        except (ValueError, OSError):
            continue
        for sock in readable:
            if sock is sys.stdin:
                line = sys.stdin.readline()
                if line == "":
                    continue
                handle_command(state, line.strip())
            elif sock is state.tcp_listen:
                handlers.accept_connection(state)
            elif sock in state.pending_handshakes:
                handlers.process_handshake(state, sock)
            else:
                handlers.handle_neighbor_socket(state, sock)


def parse_args():
    parser = argparse.ArgumentParser(description="Peer node for p2pnet", add_help=False)
    parser.add_argument("-s", default="192.168.56.21", help="Peer server address")
    parser.add_argument("-p", type=int, default=58000, help="Peer server UDP port")
    parser.add_argument("-l", type=int, required=True, help="Local TCP listening port")
    parser.add_argument("-n", type=int, required=True, help="Maximum number of neighbors")
    parser.add_argument("-h", type=int, required=True, dest="hc", help="Hopcount for searches")
    args = parser.parse_args()
    if args.l <= 0 or args.l > 65535 or args.p <= 0 or args.p > 65535:
        parser.error("Ports must be between 1 and 65535")
    if args.n <= 0 or args.hc <= 0:
        parser.error("Values for neigh and hopcount must be greater than 0")
    return args


def main():
    args = parse_args()
    state = PeerState(args)
    try:
        run(state)
    except KeyboardInterrupt:
        command_leave(state)


if __name__ == "__main__":
    main()
