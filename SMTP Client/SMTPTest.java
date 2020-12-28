/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtpclientprogram;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author lelmil
 */
public class SMTPTest {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {
        // TODO code application logic here
        Scanner sc = new Scanner(System.in);
        Context context = new Context();
        File users_list_file = new File("files/users_list.txt");
        String msg_body_filename = "files/msg_body.txt";
        AttachmentAdder.handleAtt();
        
        context.doJob("dummy");
        while(true){
            context.state.printState();
            String command = sc.nextLine();
            if("HELO".equals(command.toUpperCase())){
                try {
                    command = "HELO " + InetAddress.getLocalHost();
                    context.doJob(command);
                } catch (UnknownHostException ex) {
                    Logger.getLogger(SMTPTest.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            else if("MAIL".equals(command.toUpperCase())){
                FileInputStream fis_users = new FileInputStream(users_list_file);
                BufferedReader  br_users = new BufferedReader(new InputStreamReader(fis_users));
                String mail = br_users.readLine();
                context.doJob("MAIL FROM:<" + mail + ">");     
            }
            else if("RCPT".equals(command.toUpperCase())){
                FileInputStream fis_users = new FileInputStream(users_list_file);
                BufferedReader  br_users = new BufferedReader(new InputStreamReader(fis_users));
                br_users.readLine(); // skip sender
                String mail = br_users.readLine();
                while(mail!=null){
                    context.doJob("RCPT TO:<" + mail + ">");
                    mail = br_users.readLine();
                }
            }
            else if("DATA".equals(command.toUpperCase())){
                context.doJob("DATA");
                context.state.printState();
                context.doJob(msg_body_filename);
                //break;
            }
            else{
                context.doJob(command);
            }
            if(context.abortFlag==true){
                System.out.print("Program aborted. Try again? (Y/n)");
                String cmd = sc.nextLine();
                if(cmd.startsWith("y") || cmd.startsWith("Y")){
                    AttachmentAdder.handleAtt();
        
                    context.doJob("dummy");
                    context.abortFlag = false;
                }
                else if(cmd.startsWith("N") || cmd.startsWith("n")){
                    break;
                }
                else{
                    System.out.println("Invalid input.");
                    break;
                }
                
            }
        }
    }
    
}
