import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import serial
import serial.tools.list_ports  # Requires pyserial
import threading
import time

ser = None
connected = False
steppers_enabled = False
# y_min = 0
# y_max = 5


# Function to close the window and clean up resources
def on_closing():
    if ser and ser.is_open:
        ser.close()
        print("Serial port closed.")
    root.destroy()
    quit()


# Function to scan for available COM ports
def scan_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]


# Function to establish a serial connection
def connect_serial():
    global ser
    global connected
    com = com_port_var.get()
    baud = baud_rate_var.get()
    try:
        ser = serial.Serial(com, baud, timeout=1)
        time.sleep(2)  # wait for the serial connection to initialize
        print(f"Connected to {com} at {baud} baud.")
        connected = True
    except Exception as e:
        print("Error in serial connection:", e)


def disconnect_serial():
    global ser
    global connected
    try:
        ser.close()
        print(f"Disconnected")
        connected = False
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
    global connected
    while True:
        if ser and connected:
            try:
                line = ser.readline().decode("utf-8").strip()
                if line:
                    print("Received:", line)
                    update_gui(line)
                    update_data_buffer(line)
            except Exception as e:
                print("Error reading from serial:", e)
        # time.sleep(0.1)


# Function to toggle stepper motors
def toggle_stepper():
    global steppers_enabled
    global connected
    if connected:
        if not steppers_enabled:
            send_command("RUN#")
            stepper_toggle_button.config(text="Disable Steppers")
            steppers_enabled = True
        else:
            send_command("STOP#")
            stepper_toggle_button.config(text="Enable Steppers")
            steppers_enabled = False


# Function to send a command to the Arduino
def send_command(command):
    if ser and ser.is_open:
        ser.write(command.encode())
        print(f"Sent: {command}")


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
    output_line = None  # Variable to store the "Output" series if selected

    for key in plot_data_checkboxes:
        if plot_data_checkboxes[key].get():
            if key == "Output":
                output_line = data_buffer[key][-num_data_points.get() :]
            else:
                lines.append(data_buffer[key][-num_data_points.get() :])

    if lines:
        lines = np.array(lines)
        ax.clear()
        ax.plot(lines.transpose())
        if not l_auto_range_var.get():
            ax.set_ylim(l_y_min.get(), l_y_max.get())

    if output_line is not None:
        ax2.clear()
        ax2.plot(output_line)
        if not r_auto_range_var.get():
            ax2.set_ylim(r_y_min.get(), r_y_max.get())

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


# Function to resize the plot when the window size changes
def resize_plot(event):
    # Update the canvas size to match the new window size
    canvas_widget.config(width=event.width - 10, height=event.height - 10)


# Create the main window
root = tk.Tk()
root.title("BalanceBot Control Panel")
root.geometry("800x900")

# Frame for connection settings
connection_frame = ttk.Frame(root)
connection_frame.grid(row=0, column=0, columnspan=3, padx=10, pady=5, sticky="ew")

# COM Port and Baud Rate Selection
com_port_var = tk.StringVar()
baud_rate_var = tk.StringVar(value="115200")  # Default baud rate

com_ports = scan_ports()
com_port_cb = ttk.Combobox(
    connection_frame, textvariable=com_port_var, values=com_ports
)
com_port_cb.grid(row=0, column=0, padx=10, pady=5)
com_port_cb.set("COM3")  # Placeholder text

baud_rates = [
    "9600",
    "14400",
    "19200",
    "38400",
    "57600",
    "115200",
    "230400",
    "460800",
    "921600",
]
baud_rate_cb = ttk.Combobox(
    connection_frame, textvariable=baud_rate_var, values=baud_rates
)
baud_rate_cb.grid(row=0, column=1, padx=10, pady=5)

connect_button = ttk.Button(connection_frame, text="Connect", command=connect_serial)
connect_button.grid(row=0, column=2, padx=10, pady=5)

disconnect_button = ttk.Button(
    connection_frame, text="Disconnect", command=disconnect_serial
)
disconnect_button.grid(row=0, column=3, padx=10, pady=5)

# Frame for input/output fields
io_frame = ttk.Frame(root)
io_frame.grid(row=1, column=0, columnspan=3, padx=10, pady=5, sticky="ew")

# Define parameter codes and data fields
params = {
    "AngKP": "AKP",
    "AngKI": "AKI",
    "AngKD": "AKD",
    "TiltOffset": "ASP",
    "maxAngle": "ALM",
    "MaxSpeed": "AFQ",
    "PosKP": "PKP",
    "PosKI": "PKI",
    "PosKD": "PKD",
    "PosSP": "PSP",
    "MaxTilt": "ALM",
}
data_fields = ["Output", "Bz", "My", "Setpoint", "Ang", "Pos"]
data_labels = {}

# Create input fields using grid layout
for i, (param, code) in enumerate(params.items()):
    label = ttk.Label(io_frame, text=param)
    label.grid(row=i, column=0, padx=10, pady=5, sticky="e")  # Align label to right

    entry = ttk.Entry(io_frame, width=8)
    entry.grid(row=i, column=1, padx=10, pady=5, sticky="w")  # Align entry to left
    entry.bind("<FocusOut>", lambda event, c=code: validate_and_send(event, c))

# Create labels for data display in a separate loop
for i, field in enumerate(data_fields):
    data_label = ttk.Label(io_frame, text=f"{field}: N/A")
    data_label.grid(row=i, column=2, padx=10, pady=5, sticky="w")
    data_labels[field] = data_label

# stepper_toggle_var = tk.BooleanVar()
stepper_toggle_button = ttk.Button(
    io_frame, text="Enable Steppers", command=toggle_stepper
)
stepper_toggle_button.grid(row=len(params) + 1, column=1)

# Create checkboxes for selecting data to plot
plot_data_checkboxes = {}
for i, field in enumerate(data_fields):
    var = tk.BooleanVar(value=True)
    chk = ttk.Checkbutton(io_frame, text=field, variable=var, command=update_plot)
    chk.grid(row=i, column=3, sticky="w")
    plot_data_checkboxes[field] = var

# Frame for plotting
plot_frame = ttk.Frame(root)
plot_frame.grid(row=2, column=0, sticky="nsew")
root.grid_rowconfigure(2, weight=1)  # Allow plot_frame to expand
root.grid_columnconfigure(0, weight=1)  # Allow plot_frame to expand horizontally

# Field for specifying number of data points
num_data_points_label = ttk.Label(plot_frame, text="Max Data Points:")
num_data_points_label.grid(
    row=0, column=0, padx=10, pady=5, sticky="e"
)  # Align label to right
num_data_points = tk.IntVar(value=50)
num_points_entry = ttk.Entry(plot_frame, textvariable=num_data_points, width=4)
num_points_entry.grid(row=0, column=1, sticky="w")
l_y_min = tk.DoubleVar(value=0)
l_y_min_label = ttk.Label(plot_frame, text="yMinLeft:")
l_y_min_label.grid(row=0, column=2, padx=10, pady=5, sticky="e")  # Align label to right
l_y_min_entry = ttk.Entry(plot_frame, textvariable=l_y_min, width=4)
l_y_min_entry.grid(row=0, column=3, sticky="w")
l_y_max = tk.DoubleVar(value=5)
l_y_max_label = ttk.Label(plot_frame, text="yMaxLeft:")
l_y_max_label.grid(row=0, column=4, padx=10, pady=5, sticky="e")  # Align label to right
l_y_max_entry = ttk.Entry(plot_frame, textvariable=l_y_max, width=4)
l_y_max_entry.grid(row=0, column=5, sticky="w")
l_auto_range_var = tk.BooleanVar(value=True)
l_auto_range_chk = ttk.Checkbutton(
    plot_frame, text="AutoRangeLeft", variable=l_auto_range_var
)
l_auto_range_chk.grid(row=0, column=6, sticky="w")
r_y_min = tk.DoubleVar(value=0)
r_y_min_label = ttk.Label(plot_frame, text="yMinRight:")
r_y_min_label.grid(row=0, column=7, padx=10, pady=5, sticky="e")  # Align label to right
r_y_min_entry = ttk.Entry(plot_frame, textvariable=r_y_min, width=4)
r_y_min_entry.grid(row=0, column=8, sticky="w")
r_y_max = tk.DoubleVar(value=5)
r_y_max_label = ttk.Label(plot_frame, text="yMaxRight:")
r_y_max_label.grid(row=0, column=9, padx=10, pady=5, sticky="e")  # Align label to right
r_y_max_entry = ttk.Entry(plot_frame, textvariable=r_y_max, width=4)
r_y_max_entry.grid(row=0, column=10, sticky="w")
r_auto_range_var = tk.BooleanVar(value=True)
r_auto_range_chk = ttk.Checkbutton(
    plot_frame, text="AutoRangeRight", variable=r_auto_range_var
)
r_auto_range_chk.grid(row=0, column=11, sticky="w")

# Initialize the plot
fig, ax = plt.subplots()
ax2 = ax.twinx()
ax.set_ylim(l_y_min.get(), l_y_max.get())
canvas = FigureCanvasTkAgg(fig, master=plot_frame)
canvas_widget = canvas.get_tk_widget()
canvas_widget.grid(row=1, column=0, columnspan=12, sticky="ew")

# Initialize a buffer for storing data
data_buffer = {field: [] for field in data_fields}

# Bind the resize event to the resize_plot function
plot_frame.bind("<Configure>", resize_plot)

# Start a new thread for reading data from Arduino
thread = threading.Thread(target=read_from_arduino, daemon=True)
thread.start()

# Bind the closing event of the window
root.protocol("WM_DELETE_WINDOW", on_closing)

# Start the GUI event loop
root.mainloop()

# # Close the serial connection when the GUI is closed, if it was opened
# if ser:
#     ser.close()
