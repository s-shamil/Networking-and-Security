BEGIN{
	n_sent_packets = 0;
	n_recv_packets = 0;
	n_drop_packets = 0;
	n_recv_bytes = 0;
	r_start_time = 10000.0;
	r_end_time = 0.0;
}

{
	event = $1;
	rTime = $2;
	node1 = $3;
	node2 = $4;
	type = $5;
	packetSize = $6;
	if(type == "cbr"){
	if(rTime>r_end_time) r_end_time=rTime;
	if(rTime<r_start_time) r_start_time=rTime;
	
	if(event == "r"){
		n_recv_packets += 1;
		n_recv_bytes += packetSize;
	}
	
	else if(event == "+"){
		n_sent_packets += 1;
	}
	
	else if(event == "d"){
		n_drop_packets += 1;
	}
	}	
}

END {
	rTime = r_end_time - r_start_time;
	packetDeliveryRatio = n_recv_packets / n_sent_packets * 100;	
	packetDropRatio = n_drop_packets / n_sent_packets * 100;
	rThroughput = (n_recv_bytes*8) / rTime;

	printf("%f\n",rThroughput);
	printf("%f\n",(100-packetDropRatio));
	printf("%f\n",packetDropRatio);
	
	
}	
