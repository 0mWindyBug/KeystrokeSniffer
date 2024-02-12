# KeystrokeSniffer 
KeystrokeSniffer is a filter driver that allows you to record and log keystrokes on a windows victim 

Tested on Windows 10 21H2 and 22H2

# Usage:
under 'log.h' you can configure the path to the log file where KeystrokeSniffer will write recorded keystrokes 

# Keystroke processing for PS/2 keyboards 
when a key is pressed or released , an interrupt (IRQ1) is generated and handled by an ISR implemented by the i8042prt.sys driver.
the ISR is responsible to read the scancode and queue a DPC which in turn will invoke the KeyboardClassServiceCallback, implemented by the KeyboardClass driver. 
simultaneously , the raw input thread (hosted by csrss.exe and implemented as part of win32k.sys) routinely sends read IRPs to the KeyboardClass driver , those IRPs are marked as pending .
the service callback , which is invoked as a response to the a key press/release , is responsible to trigger the completion processing of the pending IRP. 
the KeyboardClass driver completion routine for IRP_MJ_READ will deliver the data (over an apc) back to the RIT
# Keystroke processing for HID keyboards 
whilst the RIT - KeyboardClass processing remains pretty much the same , there's a noteable difference in the implementation of the initial processing of the keystroke  
instead of delivering an interrupt whenever there's a new keyboard event to process, the operating system (specifically HID related driver) check periodcally for those events 
the kbhid.sys driver queues read IRPs to the HidClass driver , which operates against the hardware to detect new keyboard events and transmists "HID packets" back up the device stack 
kbhid.sys , along with HidParse will convert those HID packets into an array of KEYBOARD_INPUT_DATA structures. lastly , it will invoke the keyboard service callback

# How it works 
we start by creating a device object and attaching it to the KeyboardClass0 device stack
we set a completion routine for IRP_MJ_READ , where the scancode and flags about the event are extracted from the KEYBOARD_INPUT_DATA structures and saved into a global linked list 
due to IRQL restrictions , synchronization of the list is implemented via a spin lock.
a dedicated system thread is created to take care of writing recorded keystrokes back to our log file 
whenever the number of keys in the list reaches a certian limit (currently set to 100) the writer thread is signaled and executes the write operation 
unload trigger a write operation regardless of the number of keys in the list , so we dont "lose" recorded keys




  
