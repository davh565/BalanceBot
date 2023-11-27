import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import serial
import threading
import time

# Initialize Serial Connection
ser = None
try:
    ser = serial.Serial("COM3", 115200, timeout=1)
    time.sleep(2)  # wait for the serial connection to initialize
except Exception as e:
    print("Error in serial connection:", e)


# Function to validate and send data to Arduino when the field loses focus
def validate_and_send(event, code):
    value = event.widget.get()
    try:
        # Try to convert the value to a float
        float_value = float(value)
        if ser:
            try:
                message = f"{code}:{float_value}#\r"
                # Send the data
                ser.write(message.encode())
                print("Sent:", message)
            except Exception as e:
                print("Error:", e)
    except ValueError:
        # Clear the entry if it's not a valid float
        event.widget.delete(0, tk.END)
        print(f"Cleared invalid entry for {code}.")


# Function to read data from Arduino and update GUI
def read_from_arduino():
    while True:
        if ser:
            try:
                line = ser.readline().decode("utf-8").strip()
                if line:
                    print("Received:", line)
                    update_gui(line)
                    update_data_buffer(line)
            except Exception as e:
                print("Error reading from serial:", e)
        time.sleep(0.1)


# Function to update GUI with received data
def update_gui(data):
    try:
        data_fields = data.split(", ")
        for field in data_fields:
            key, value = field.split(":")
            if key in data_labels:
                data_labels[key].config(text=f"{key}: {value}")
    except Exception as e:
        print("Error updating GUI:", e)


# Function to update the plot
def update_plot():
    global data_buffer
    lines = []
    for key in plot_data_checkboxes:
        if plot_data_checkboxes[key].get():
            lines.append(data_buffer[key][-num_data_points.get() :])
    if lines:
        lines = np.array(lines)
        ax.clear()
        ax.plot(lines.transpose())
        canvas.draw()


# Function to update the data buffer for plotting
def update_data_buffer(line):
    try:
        data_fields = line.split(", ")
        for field in data_fields:
            key, value = field.split(":")
            data_buffer[key].append(float(value))
            # Trim the buffer to the last num_data_points
            data_buffer[key] = data_buffer[key][-num_data_points.get() :]
        update_plot()
    except Exception as e:
        print("Error updating data buffer:", e)


# Create the main window
root = tk.Tk()
root.title("Arduino Control Panel")

# Define parameter codes and data fields
params = {
    "KP": "KP",
    "KI": "KI",
    "KD": "KD",
    "Setpoint": "SP",
    "Limits": "LM",
    "MaxSpeed": "FQ",
}
data_fields = ["Output", "y", "z", "Setpoint", "Gravity"]
data_labels = {}

# Create input fields using grid layout
for i, (param, code) in enumerate(params.items()):
    label = ttk.Label(root, text=param)
    label.grid(row=i, column=0, padx=10, pady=5, sticky="e")  # Align label to right

    entry = ttk.Entry(root)
    entry.grid(row=i, column=1, padx=10, pady=5, sticky="w")  # Align entry to left
    entry.bind("<FocusOut>", lambda event, c=code: validate_and_send(event, c))

# Create labels for data display in a separate loop
for i, field in enumerate(data_fields):
    data_label = ttk.Label(root, text=f"{field}: N/A")
    data_label.grid(row=i, column=2, padx=10, pady=5, sticky="w")
    data_labels[field] = data_label

# Section for plotting
plot_frame = ttk.Frame(root)
plot_frame.grid(row=len(params), column=0, columnspan=3, sticky="ew", padx=10, pady=10)

# Create checkboxes for selecting data to plot
plot_data_checkboxes = {}
for i, field in enumerate(data_fields):
    var = tk.BooleanVar(value=True)
    chk = ttk.Checkbutton(plot_frame, text=field, variable=var, command=update_plot)
    chk.grid(row=0, column=i, sticky="w")
    plot_data_checkboxes[field] = var

# Field for specifying number of data points
num_data_points = tk.IntVar(value=50)
num_points_entry = ttk.Entry(plot_frame, textvariable=num_data_points)
num_points_entry.grid(row=0, column=len(data_fields), sticky="w")

# Initialize the plot
fig, ax = plt.subplots()
canvas = FigureCanvasTkAgg(fig, master=plot_frame)
canvas_widget = canvas.get_tk_widget()
canvas_widget.grid(row=1, column=0, columnspan=len(data_fields) + 1, sticky="ew")

# Initialize a buffer for storing data
data_buffer = {field: [] for field in data_fields}

# Start a new thread for reading data from Arduino
thread = threading.Thread(target=read_from_arduino, daemon=True)
thread.start()

# Start the GUI event loop
root.mainloop()

# Close the serial connection when the GUI is closed, if it was opened
if ser:
    ser.close()
