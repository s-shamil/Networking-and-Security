/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtpclientprogram;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.util.Base64;

/**
 *
 * @author lelmil
 */
public class AttachmentAdder {

    public static void prepareWithAttachment(boolean flag) throws FileNotFoundException, IOException {
        File at = new File("files/attachments.txt");
        File mt = new File("files/msg_text.txt");
        
        String msg_body_file = "files/msg_body.txt";

        BufferedReader br_at = new BufferedReader(new InputStreamReader(new FileInputStream(at)));
        BufferedReader br_mt = new BufferedReader(new InputStreamReader(new FileInputStream(mt)));

        String line, fullmsg = "";
        boolean header_completed = false;
        while ((line = br_mt.readLine()) != null) {
            if (line.isEmpty() && !header_completed && flag) {
                fullmsg = fullmsg
                        + "MIME-Version: 1.0\n"
                        + "Content-type: multipart/mixed; boundary=\"separator\"\n"
                        + "\n";
                fullmsg = fullmsg + "This is the preamble.  It is to be ignored, though it is a handy place for mail composers to include an explanatory note to non-MIME compliant readers.\n--separator\n\n" ;
                header_completed = true;
            } else if (line.equals(".")) {
                if (flag) {
                    String attFileName;  
                    while ((attFileName = br_at.readLine()) != null) {
                        fullmsg = fullmsg
                                + "--separator\n"
                                + "Content-Transfer-Encoding: Base64\n"
                                + "Content-Disposition: attachment; filename=" + attFileName + "\n\n";
                        File file = new File("files/" + attFileName);
                        byte[] encoded = Files.readAllBytes(file.toPath());
                        String img_code = Base64.getMimeEncoder().encodeToString(encoded);
                        fullmsg = fullmsg + img_code + "\n";
                    }
                    fullmsg+="--separator--\n";
                }
                fullmsg = fullmsg + ".\n";
            } else {
                fullmsg += (line + "\n");
            }
        }
        File output = new File(msg_body_file);
        PrintWriter pw = new PrintWriter(output);
        pw.write(fullmsg);
        pw.close();
        br_at.close();
        br_mt.close();
    }

    public static void handleAtt() throws FileNotFoundException, IOException {
        File at = new File("files/attachments.txt");
        BufferedReader br_at = new BufferedReader(new InputStreamReader(new FileInputStream(at)));
        //check to add attachment
        String line;
        boolean flag = false;
        int att_cnt = 0;
        while ((line = br_at.readLine()) != null && !line.isEmpty()) {
            att_cnt++;
        }

        if (att_cnt > 0) {
            flag = true;
        }
        prepareWithAttachment(flag);
    }
}
