#!/usr/bin/env python3
"""
Simple serial data reader: reads from /dev/ttyACM0 and saves to a file.
Usage: python3 serial_reader.py [output_file]
"""

import sys
import serial
import serial.tools.list_ports
import time


def find_serial_port():
    """Find the first available ACM/USB serial port."""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "Pico" in port.description:
            print(f"[+] Found serial port: {port.device}")
            return port.device
    print("[-] No ACM/USB serial ports found.")
    return None


def read_serial(port, filename, baudrate=115200, timeout=0.1):
    """Read serial data and save to file."""
    if not port:
        print("[-] No serial port specified or found.")
        return

    print(f"[+] Opening {port} at {baudrate} baud...")
    set = serial.Serial(port, baudrate, timeout=timeout)

    # Give hardware time to initialize
    time.sleep(0.5)

    output_file = (
        sys.argv[1] if len(sys.argv) > 1 else "recordings/" + filename + ".csv"
    )
    print(f"[+] Writing to {output_file}")

    try:
        with open(output_file, "wb") as f:
            while True:
                data = set.read(set.in_waiting or 1)
                if data:
                    f.write(data)
        print("[+] Done!")
    except KeyboardInterrupt:
        print("\n[+] Interrupted by user.")
    finally:
        set.close()
        print(f"[+] Serial port closed.")


if __name__ == "__main__":
    filename = input("Enter name for recording: ")
    port = find_serial_port()
    if port:
        read_serial(port, filename)
