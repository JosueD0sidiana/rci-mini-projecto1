# p2p_utils/utils.py
import socket


def create_listen_socket(lnkport):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", lnkport))
    sock.listen(5)
    sock.setblocking(False)
    return sock


def send_line(sock, text):
    sock.sendall((text + "\n").encode("ascii"))


def safe_close(sock):
    try:
        sock.close()
    except Exception:
        pass
