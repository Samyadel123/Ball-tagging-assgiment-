import socket
import json
import time
import random
import threading
import sys

# Constants
COURT_LENGTH = 100.0
COURT_WIDTH = 100.0
NUM_BALLS = 50
GAME_DURATION_SEC = 10
BALL_POSITIONS = []
BALL_COLORS = [0] * NUM_BALLS 

def handle_player(conn, addr, player_id, locks):
    print(f"[HOST] Connected to Player {player_id}")
    try:
        # Send initial state using a robust format
        init_data = {
            "num_balls": NUM_BALLS,
            "duration": GAME_DURATION_SEC,
            "positions": BALL_POSITIONS
        }
        conn.sendall((json.dumps(init_data) + "\n").encode())

        while True:
            data = conn.recv(1024)
            if not data: break
            
            msgs = data.decode().split('\n')
            for msg in msgs:
                if msg.startswith("TAG"):
                    parts = msg.split()
                    if len(parts) < 3: continue
                    idx, color = int(parts[1]), int(parts[2])
                    
                    # Paradigm Logic
                    with locks[idx]:
                        if BALL_COLORS[idx] == 0:
                            BALL_COLORS[idx] = color
                            conn.sendall(b"SUCCESS\n")
                        else:
                            conn.sendall(b"FAILURE\n")
    except Exception as e:
        pass
    finally:
        conn.close()

def main():
    global BALL_POSITIONS
    BALL_POSITIONS = [[random.uniform(0, COURT_LENGTH), random.uniform(0, COURT_WIDTH)] for _ in range(NUM_BALLS)]
    locks = [threading.Lock() for _ in range(NUM_BALLS)]

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('localhost', 5000))
    server.listen(2)
    print("[HOST] Started. Waiting for A (Java) and B (C++)...")

    t1 = threading.Thread(target=lambda: handle_player(*server.accept(), 'A', locks))
    t2 = threading.Thread(target=lambda: handle_player(*server.accept(), 'B', locks))
    t1.start()
    t2.start()

    time.sleep(GAME_DURATION_SEC)
    print("\n[HOST] Game Over.")
    red = BALL_COLORS.count(1)
    blue = BALL_COLORS.count(2)
    print(f"Red (Java): {red}, Blue (C++): {blue}, White: {BALL_COLORS.count(0)}")
    sys.exit(0)

if __name__ == "__main__":
    main()
