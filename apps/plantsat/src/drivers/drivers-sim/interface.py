import zmq

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")

try:
    while True:
        message = socket.recv()
        print(f"Received request: {message}")
        #  Send reply back to client
        socket.send(message)
except Exception as e:
    print(e)
finally:
    socket.close()
    context.destroy()
