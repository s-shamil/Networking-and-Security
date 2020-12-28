import binascii
import socket
import time


def send_udp_message(message, address, port):
    """send_udp_message sends a message to UDP server

    message should be a hexadecimal encoded string
    """
    message = message.replace(" ", "").replace("\n", "")
    server_address = (address, port)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.sendto(binascii.unhexlify(message), server_address)
        print('sent one')
        send_time = time.time()
        data, _ = sock.recvfrom(4096)
        recv_time = time.time()
        print ('time needed : ' + str(recv_time - send_time))
    finally:
        sock.close()
    return binascii.hexlify(data).decode("utf-8")


def format_hex(hex):
    """format_hex returns a pretty version of a hex string"""
    octets = [hex[i:i+2] for i in range(0, len(hex), 2)]
    pairs = [" ".join(octets[i:i+2]) for i in range(0, len(octets), 2)]
    return "\n".join(pairs)


message = "AA AA 01 00 00 01 00 00 00 00 00 00 " \
"14 6d 6f 74 68 65 72 66 75 63 6b 69 6e 67 77 65 62 73 69 74 65 03 63 6f 6d 00 00 01 00 01"

#response = send_udp_message(message, "localhost", 600)
response = send_udp_message(message, "8.8.8.8", 53)
#print(format_hex(response))
print('~~~got response~~~')