# peer_server.py
import argparse
import socket
import sys


def parse_args():
    parser = argparse.ArgumentParser(description="Peer server for p2pnet")
    parser.add_argument("-p", type=int, default=58000, help="UDP port to listen on")
    return parser.parse_args()


def main():
    args = parse_args()
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind(("", args.p))
    except OSError as exc:
        sys.stderr.write("Failed to bind UDP port {}: {}\n".format(args.p, exc))
        sys.exit(1)

    peers = {}
    next_seqnumber = 1

    while True:
        try:
            data, addr = sock.recvfrom(4096)
        except KeyboardInterrupt:
            break
        except OSError:
            continue

        try:
            text = data.decode("ascii", errors="ignore").strip()
        except Exception:
            continue

        if not text:
            continue

        parts = text.split()
        cmd = parts[0]
        response = "NOK\n"

        if cmd == "REG" and len(parts) == 2:
            try:
                lnkport = int(parts[1])
                if lnkport <= 0 or lnkport > 65535:
                    raise ValueError
                seq = next_seqnumber
                peers[seq] = (addr[0], lnkport)
                next_seqnumber += 1
                response = "SQN {}\n".format(seq)
            except Exception:
                response = "NOK\n"
        elif cmd == "UNR" and len(parts) == 2:
            try:
                seq = int(parts[1])
                if seq in peers:
                    del peers[seq]
                    response = "OK\n"
                else:
                    response = "NOK\n"
            except Exception:
                response = "NOK\n"
        elif cmd == "PEERS" and len(parts) == 1:
            lines = ["LST"]
            for seq, (ip, lnkport) in peers.items():
                lines.append("{}:{}#{}".format(ip, lnkport, seq))
            lines.append("")
            response = "\n".join(lines) + "\n"

        try:
            sock.sendto(response.encode("ascii", errors="ignore"), addr)
        except OSError:
            continue


if __name__ == "__main__":
    main()
