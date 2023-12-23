import tkinter as tk
from tkinter import ttk

# Create the main window
root = tk.Tk()
root.title("BalanceBot Control Panel")
root.geometry("800x900")

########################################################################################
# Frame for connection settings
connection_frame = ttk.Frame(root, relief="groove")
connection_frame.grid(row=0, column=0, sticky="ew")


# COM Port and Baud Rate Selection
com_port_var = tk.StringVar()
baud_rate_var = tk.StringVar(value="115200")  # Default baud rate

com_ports = ["tst", "tst2"]
com_port_cb = ttk.Combobox(
    connection_frame, textvariable=com_port_var, values=com_ports
)
com_port_cb.grid(row=0, column=0, padx=10, pady=5)
com_port_cb.set("COM3")  # Placeholder text

########################################################################################
# Frame for input/output fields
io_frame = ttk.Frame(root, relief="groove")
io_frame.grid(row=1, column=0, sticky="ew")

connect_button = ttk.Button(io_frame, text="Connect")
connect_button.grid(
    row=0,
    column=0,
)
########################################################################################
# Frame for plotting
plot_frame = ttk.Frame(root, relief="groove")
plot_frame.grid(row=2, column=0, sticky="nsew")
# root.grid_rowconfigure(2, weight=1)  # Allow plot_frame to expand
# root.grid_columnconfigure(0, weight=1)  # Allow plot_frame to expand horizontally

# Bind the closing event of the window
# root.protocol("WM_DELETE_WINDOW", on_closing)

# Start the GUI event loop
root.mainloop()
