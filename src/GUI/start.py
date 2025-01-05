import ctypes
import tkinter as tk
from tkinter import filedialog
from tkinter import font
import time


def importCDLL():
    global backupLib;
    backupLib = ctypes.CDLL('./BACKUP.so')
    #add function:
    backupLib.add.argtypes = (ctypes.c_int, ctypes.c_int)
    backupLib.add.restype = ctypes.c_int
    #backup function:
    #backup(string srcLocation, string destlocation, string filters, bool compress, bool encrypt, string key)
    backupLib.backup.argtypes = (ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool, ctypes.c_bool, ctypes.c_char_p)
    backupLib.backup.restype = ctypes.c_int
    #restore function:
    #restore(const char* srcFile, const char* destLocation, bool compressed, bool encrypted, const char* key)
    backupLib.restore.argtypes = (ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool, ctypes.c_bool, ctypes.c_char_p)
    backupLib.restore.restype = ctypes.c_int
    return

def runABItest():
    result = backupLib.add(5, 40)
    print(f"result is {result}")
    print("testing backup:")
    filters = ".txt, .gif,.jpg,ero.sfc"
    result = backupLib.backup("testFolder".encode('utf-8'), "backup.pck".encode('utf-8'), filters.encode('utf-8'), False, False, "none".encode('utf-8'))
    print(f"PY: done with backup: {result}\n")
    result = backupLib.restore("backup.pck".encode('utf-8'), "./testDestFolder".encode('utf-8'), False, False, "none".encode('utf-8'))
    print(f"PY: done with backup: {result}")
    return

def makePackage():
    destPath = filedialog.asksaveasfilename(
        defaultextension=".pck",
        filetypes=[("Package files", "*.pck"), ("All files", "*.*")]
    )
    if destPath:
        print(f"File path: {destPath}")
    else:
        writeStatus("No Destination Selected", "#FF0000")
        return
    
    if(encryptState.get() and len(keyTextbox.get()) == 0):
        writeStatus("Passkey cannot be empty", "#FF0000")
        return

    srcFldr = folderLabel.cget("text")
    fltrs = filtersTextbox.get()
    cmprs = compressState.get()
    encrypt = encryptState.get()
    key = keyTextbox.get()
    writeStatus("Creating Data Package...", "#0000FF")
    root.update_idletasks()
    result = backupLib.backup(srcFldr.encode('utf-8'), destPath.encode('utf-8'), fltrs.encode('utf-8'), cmprs, encrypt, key.encode('utf-8'))
    print(f"PY: done with backup: {result}\n")
    #result status:
    if (result == 0):   #no errors
        writeStatus("Package Created Successfully", "#0fb000")
    elif(result == 1):  #enumerationError
        writeStatus("Error Enumerating Files - Please Check Path", "#FF0000")
    elif(result == 2):  #filterError
        writeStatus("Error Filtering Files - Please Check Filters", "#FF0000")
    elif(result == 3):  #packagingError
        writeStatus("Error Packaging Files", "#FF0000")
    elif(result == 4):  #compressionError
        writeStatus("Error Compressing Files", "#FF0000")
    elif(result == 5):  #encryptionError
        writeStatus("Error Encrypting Files", "#FF0000")
    elif(result == 6):  #writeError
        writeStatus("Error Writing Package to Disk", "#FF0000")
    return

def restorePackage():
    #select destintion folder:
    folderRestore = filedialog.askdirectory()
    if folderRestore:
        print(f"Selected folder: {folderRestore}")
    else:
        writeStatus("No Destination Selected", "#FF0000")
        return
    
    if(encryptState.get() and len(keyTextbox.get()) == 0):
        writeStatus("Passkey cannot be empty", "#FF0000")
        return

    srcFldr = packagePath
    cmprs = compressState.get()
    encrypt = encryptState.get()
    key = keyTextbox.get()
    writeStatus("Restoring Data From Package...", "#0000FF")
    root.update_idletasks()
    result = backupLib.restore(srcFldr.encode('utf-8'), folderRestore.encode('utf-8'), cmprs, encrypt, key.encode('utf-8'))
    print(f"PY: done with backup: {result}\n")
    #result status:
    if (result == 0):   #no errors
        writeStatus("Package Restored Successfully", "#0fb000")
    elif(result == 1):  #enumerationError
        writeStatus("Error Unpackaging File", "#FF0000")
    elif(result == 2):  #filterError
        writeStatus("Error Filtering Files", "#FF0000")
    elif(result == 3):  #packagingError
        writeStatus("Error Unpackaging File", "#FF0000")
    elif(result == 4):  #compressionError
        writeStatus("Error Decompressing File", "#FF0000")
    elif(result == 5):  #encryptionError
        writeStatus("Error Decrypting File", "#FF0000")
    elif(result == 6):  #writeError
        writeStatus("Error Reading Package From Disk", "#FF0000")
    return

def writeStatus(message, color="#000000"):
    statusLabel.config(text=message, fg=color)
    return

def selectFolder():
    global folderSelected
    folderSelected = filedialog.askdirectory()
    if folderSelected:
        print(f"Selected folder: {folderSelected}")
        folderLabel.config(text=folderSelected)
        packageButton.config(state="normal")
    else:
        folderLabel.config(text="No Folder Selected")
        packageButton.config(state="disabled")
    return

def selectDestFolder():
    global destSelected
    destSelected = filedialog.askdirectory()
    if destSelected:
        print(f"Selected folder: {destSelected}")
        destLabel.config(text=destSelected)
    return

def selectPackageFile():
    global packagePath
    packagePath = filedialog.askopenfilename(
        title="Select a Package to Restore",
        filetypes=[("Package files", "*.pck"), ("All files", "*.*")]
    )
    if packagePath:
        print(f"File path: {packagePath}")
        folderLabel.config(text=packagePath)
        packageButton.config(state="normal")
    else:
        folderLabel.config(text="No File Selected")
        packageButton.config(state="disabled")
        return
    
    
def toggleMode():
    global backupModeState
    for widget in root.winfo_children():
        widget.destroy()
    backupModeState = not(backupModeState)
    if (backupModeState):
        initBackupMode()
    else:
        initRestoreMode()
    return

def compressButtonToggle():
    return
    
def encryptButtonToggle():
    if (encryptState.get()):
        #enable key text
        keyTextbox.config(state="normal")
    else:
        #disable and clear key text
        keyTextbox.delete(0, tk.END)
        keyTextbox.config(state="disabled")
    return

def initGUI():
    global root
    root = tk.Tk()
    root.title("Generic Backup Data Software")
    root.geometry("645x480")
    global windowBackgroundColor
    windowBackgroundColor = "#EEEEEE"
    global systemFont
    systemFont = font.Font(family="DejaVu", size=12, weight="bold")
    global systemFont2
    systemFont2 = font.Font(family="DejaVu", size=12)
    
    try:
        icon = tk.PhotoImage(file="./src/GUI/icon.png")
        root.iconphoto(True, icon)
    except:
        print("Icon not found")
    
    initBackupMode()
    root.mainloop()
    return

def initBackupMode():
    #form:
    root.configure(bg=windowBackgroundColor)
    root.grid_rowconfigure(0, weight=1)
    root.grid_rowconfigure(1, weight=1)
    root.grid_rowconfigure(2, weight=1)
    root.grid_rowconfigure(3, weight=1)
    root.grid_rowconfigure(4, weight=1)
    root.grid_rowconfigure(5, weight=1)
    root.grid_rowconfigure(6, weight=1)
    root.grid_rowconfigure(7, weight=1)
    root.grid_rowconfigure(8, weight=1)
    root.grid_rowconfigure(9, weight=1)
    root.grid_rowconfigure(10, weight=1)
    root.grid_rowconfigure(11, weight=1)
    root.grid_rowconfigure(12, weight=1)
    root.grid_rowconfigure(13, weight=1)
    root.grid_rowconfigure(14, weight=1)
    root.grid_columnconfigure(0, weight=1)
    root.grid_columnconfigure(1, weight=1)
    root.grid_columnconfigure(2, weight=1)

    global fixedWidth
    fixedWidth = 215
    for i in range(3):
        frame = tk.Frame(root, width=fixedWidth, height=10, bg="#BBBBBB")
        frame.grid(row=0, column=i, sticky="nsew")
        
    #backup mode:
    global backupModeState
    backupModeState = True
    appModeButton = tk.Button(root, text="Backup Mode", command=toggleMode, font=systemFont)
    appModeButton.grid(row=0, column=0, padx=5, pady=5)
    
    #folder selector:
    selectFolderButton = tk.Button(root, text="Select Folder To Backup", command=selectFolder, font=systemFont)
    selectFolderButton.grid(row=1, column=0, padx=5, pady=5, columnspan=3, sticky="n")
    global folderLabel
    folderLabel = tk.Label(root, text="No Folder Selected", bg=windowBackgroundColor, font=systemFont2)
    folderLabel.grid(row=2, column=0, padx=5, pady=5, columnspan=3, sticky="n")
    
    #filters:
    filtersLabel = tk.Label(root, text="File Name & Extension Filters (seperate by comma)", bg=windowBackgroundColor, font=systemFont2)
    filtersLabel.grid(row=3, column=0, padx=5, pady=5, sticky="s", columnspan=3)
    global filtersTextbox
    filtersTextbox = tk.Entry(root, width=30, font=systemFont2)
    filtersTextbox.grid(row=4, column=0, padx=5, pady=5, sticky="n", columnspan=3)
    
    #compression
    global compressState
    compressState = tk.BooleanVar()
    compressCheckbutton = tk.Checkbutton(root, text="Compress Package", variable=compressState, command=compressButtonToggle, bg=windowBackgroundColor, font=systemFont)
    compressCheckbutton.grid(row=6, column=1, padx=5, pady=5)

    #encryption
    global encryptState
    encryptState = tk.BooleanVar()
    encryptCheckbutton = tk.Checkbutton(root, text="Encrypt Package", variable=encryptState, command=encryptButtonToggle, bg=windowBackgroundColor, font=systemFont)
    encryptCheckbutton.grid(row=8, column=1, padx=5, pady=5)
    keyLabel = tk.Label(root, text="Key:", bg=windowBackgroundColor, font=systemFont2)
    keyLabel.grid(row=9, column=0, padx=5, pady=5, sticky="e")
    global keyTextbox
    keyTextbox = tk.Entry(root, width=30, state="disabled", font=systemFont, show="•")
    keyTextbox.grid(row=9, column=1, padx=5, pady=5)

    #status:
    global packageButton
    packageButton = tk.Button(root, text="Create Backup Package", command=makePackage, font=systemFont, state="disabled")
    packageButton.grid(row=11, column=1, padx=5, pady=5)
    global statusLabel
    statusLabel = tk.Label(root, text="", font=systemFont, bg=windowBackgroundColor)
    statusLabel.grid(row=12, column=0, padx=5, pady=5, columnspan=3)
    return

def initRestoreMode():
    #form
    root.configure(bg=windowBackgroundColor)
    root.grid_rowconfigure(0, weight=1)
    root.grid_rowconfigure(1, weight=1)
    root.grid_rowconfigure(2, weight=1)
    root.grid_rowconfigure(3, weight=1)
    root.grid_rowconfigure(4, weight=1)
    root.grid_rowconfigure(5, weight=1)
    root.grid_rowconfigure(6, weight=1)
    root.grid_rowconfigure(7, weight=1)
    root.grid_rowconfigure(8, weight=1)
    root.grid_rowconfigure(9, weight=1)
    root.grid_rowconfigure(10, weight=1)
    root.grid_rowconfigure(11, weight=1)
    root.grid_rowconfigure(12, weight=1)
    root.grid_rowconfigure(13, weight=1)
    root.grid_rowconfigure(14, weight=1)
    root.grid_columnconfigure(0, weight=1)
    root.grid_columnconfigure(1, weight=1)
    root.grid_columnconfigure(2, weight=1)

    global fixedWidth
    fixedWidth = 215
    for i in range(3):
        frame = tk.Frame(root, width=fixedWidth, height=10, bg="#BBBBBB")
        frame.grid(row=0, column=i, sticky="nsew")
    #backup mode:
    appModeButton = tk.Button(root, text="Restore Mode", command=toggleMode, font=systemFont)
    appModeButton.grid(row=0, column=0, padx=5, pady=5)
    
    #folder selector:
    selectFolderButton = tk.Button(root, text="Select Package To Restore", command=selectPackageFile, font=systemFont)
    selectFolderButton.grid(row=1, column=0, padx=5, pady=5, columnspan=3, sticky="n")
    global folderLabel
    folderLabel = tk.Label(root, text="No File Selected", bg=windowBackgroundColor, font=systemFont2)
    folderLabel.grid(row=2, column=0, padx=5, pady=5, columnspan=3, sticky="n")
    
    #compression
    global compressState
    compressState = tk.BooleanVar()
    compressCheckbutton = tk.Checkbutton(root, text="Decompress Package", variable=compressState, command=compressButtonToggle, bg=windowBackgroundColor, font=systemFont)
    compressCheckbutton.grid(row=4, column=1, padx=5, pady=5)

    #encryption
    global encryptState
    encryptState = tk.BooleanVar()
    encryptCheckbutton = tk.Checkbutton(root, text="Decrypt Package", variable=encryptState, command=encryptButtonToggle, bg=windowBackgroundColor, font=systemFont)
    encryptCheckbutton.grid(row=6, column=1, padx=5, pady=5)
    keyLabel = tk.Label(root, text="Key:", bg=windowBackgroundColor, font=systemFont2)
    keyLabel.grid(row=7, column=0, padx=5, pady=5, sticky="e")
    global keyTextbox
    keyTextbox = tk.Entry(root, width=30, state="disabled", font=systemFont, show="•")
    keyTextbox.grid(row=7, column=1, padx=5, pady=5)

    #status:
    global packageButton
    packageButton = tk.Button(root, text="Restore Package", command=restorePackage, font=systemFont, state="disabled")
    packageButton.grid(row=9, column=1, padx=5, pady=5)
    global statusLabel
    statusLabel = tk.Label(root, text="", font=systemFont, bg=windowBackgroundColor)
    statusLabel.grid(row=10, column=0, padx=5, pady=5, columnspan=3)
    return


if __name__ == "__main__":
    importCDLL();
    #runABItest();
    initGUI()
    
