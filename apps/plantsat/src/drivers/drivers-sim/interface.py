import zmq
import random
import struct


def process_obc_request(message) -> bytes:
    if message[1] == 0x00:
        temp = random.randint(-40, 125)
        reply = struct.pack('i', temp)
        print(f"OBC temp = {temp} ºC")
    else:
        reply = message
    return reply


def process_request(message: bytes) -> bytes:
    if message[0] == 0x01:
        reply = process_obc_request(message)
    else:
        reply = message
    return reply


def server() -> None:
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5555")

    try:
        while True:
            request = socket.recv()
            print(f"Received request: {request}")
            reply = process_request(request)
            print(f"Sending reply: {reply}")
            socket.send(reply)
    except Exception as e:
        print(e)
    finally:
        socket.close()
        context.destroy()


if __name__ == '__main__':
    server()
