# p2p_utils/classes.py
import socket
from p2p_utils.utils import create_listen_socket


class Neighbor:
    def __init__(self, sock, ip, port, seq, direction):
        self.sock = sock
        self.ip = ip
        self.port = port
        self.seq = seq
        self.direction = direction


class PeerState:
    def __init__(self, args):
        self.server_addr = args.s
        self.server_port = args.p
        self.lnkport = args.l
        self.neigh = args.n
        self.hc = args.hc
        self.seqnumber = None
        self.joined = False
        self.identifiers = set()
        self.neighbors_in = {}
        self.neighbors_out = {}
        self.pending_handshakes = {}
        self.sock_buffers = {}
        self.sock_to_seq = {}
        self.local_queries = {}
        self.forwarded_queries = {}
        self.known_peers = {}
        self.udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.tcp_listen = create_listen_socket(self.lnkport)
