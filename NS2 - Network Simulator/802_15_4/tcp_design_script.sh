rm *.dat
cd out_folder
rm *.out
cd ..

#INPUT: output file AND number of iterations
output_file_format="tcp";
iteration_float=5.0;

start=1
end=4


iteration=$(printf %.0f $iteration_float);

r=$start

while [ $r -le $end ] #for things to change
do
echo "total iteration: $iteration"
###############################START A ROUND
l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;

energy_file="energy_data_$r.dat"
thr_file="thr_data_$r.dat"
delay_file="delay_data_$r.dat"
deliveryratio_file="deliveryratio_data_$r.dat"
dropratio_file="dropratio_data_$r.dat"

i=1
while [ $i -le $iteration ]
do
#################START AN ITERATION
echo "                             EXECUTING $(($i)) th ITERATION"

totalnodes=0
totalflow=0
speed=0
if [ "$r" == "1" ]; then
	packet_rate=100;
	cbr=$(echo "scale=5; 1/$packet_rate" | bc)
	totalnodes=$((10*$(($i*2))))
	ns wireless802_15.tcl 10 $(($i*2)) 10 $cbr 5   #chng
elif [ "$r" == "2" ]; then
	packet_rate=100;
	cbr=$(echo "scale=5; 1/$packet_rate" | bc)
	totalflow=$(($i*5))
	ns wireless802_15.tcl 25 4 $(($i*5)) $cbr 5 #chng
elif [ "$r" == "3" ]; then
	pow=$(($i-1))
	packet_rate=$((10**$pow));
	cbr=$(echo "scale=5; 1/$packet_rate" | bc)
	ns wireless802_15.tcl 5 5 5 $cbr 5
elif [ "$r" == "4" ]; then
	packet_rate=100;
	cbr=$(echo "scale=5; 1/$packet_rate" | bc)
	#change value of rs_thresh
	speed=$(($i*5))
	ns wireless802_15.tcl 10 5 10 $cbr $speed
fi

echo "SIMULATION COMPLETE. BUILDING STAT......"
#awk -f rule_th_del_enr_tcp.awk 802_15_grid_tcp_with_energy_random_traffic.tr > math_model1.out
awk -f wireless802_15.awk wireless802_15.tr > tcp_wireless.out


while read val
do
#	l=$(($l+$inc))
	l=$(($l+1))

	#dir="/home/ubuntu/ns2_data/iTCP/"
	#dir=""
	under="_"
	all="all"
	output_file="$output_file_format$under$r$under$r$under$all.out"


#	echo -ne "Throughput:          $thr " > $output_file

	if [ "$l" == "1" ]; then
		thr=$(echo "scale=5; $thr+$val/$iteration_float" | bc)
		echo -ne "throughput: $val " >> $output_file
		if [ "$r" == "1" ]; then
			x=$totalnodes
		elif [ "$r" == "2" ]; then
			x=$totalflow
		elif [ "$r" == "3" ]; then
			x=$packet_rate
		elif [ "$r" == "4" ]; then
			x=$speed
		fi
		echo -ne "$x $val\n" >> $thr_file
	elif [ "$l" == "2" ]; then
		del=$(echo "scale=5; $del+$val/$iteration_float" | bc)
		echo -ne "delay: $val " >> $output_file
		if [ "$r" == "1" ]; then
			x=$totalnodes
		elif [ "$r" == "2" ]; then
			x=$totalflow
		elif [ "$r" == "3" ]; then
			x=$packet_rate
		elif [ "$r" == "4" ]; then
			x=$speed
		fi
		echo -ne "$x $val\n" >> $delay_file
	elif [ "$l" == "3" ]; then
		s_packet=$(echo "scale=5; $s_packet+$val/$iteration_float" | bc)
		echo -ne "send packet: $val " >> $output_file
	elif [ "$l" == "4" ]; then
		r_packet=$(echo "scale=5; $r_packet+$val/$iteration_float" | bc)
		echo -ne "received packet: $val " >> $output_file
	elif [ "$l" == "5" ]; then
		d_packet=$(echo "scale=5; $d_packet+$val/$iteration_float" | bc)
		echo -ne "drop packet: $val " >> $output_file
	elif [ "$l" == "6" ]; then
		del_ratio=$(echo "scale=5; $del_ratio+$val/$iteration_float" | bc)
		echo -ne "delivery ratio: $val " >> $output_file
		if [ "$r" == "1" ]; then
			x=$totalnodes
		elif [ "$r" == "2" ]; then
			x=$totalflow
		elif [ "$r" == "3" ]; then
			x=$packet_rate
		elif [ "$r" == "4" ]; then
			x=$speed
		fi
		echo -ne "$x $val\n" >> $deliveryratio_file
	elif [ "$l" == "7" ]; then
		dr_ratio=$(echo "scale=5; $dr_ratio+$val/$iteration_float" | bc)
		echo -ne "drop ratio: $val " >> $output_file
		if [ "$r" == "1" ]; then
			x=$totalnodes
		elif [ "$r" == "2" ]; then
			x=$totalflow
		elif [ "$r" == "3" ]; then
			x=$packet_rate
		elif [ "$r" == "4" ]; then
			x=$speed
		fi
		echo -ne "$x $val\n" >> $dropratio_file
	elif [ "$l" == "8" ]; then
		time=$(echo "scale=5; $time+$val/$iteration_float" | bc)
		echo -ne "time: $val " >> $output_file
	elif [ "$l" == "9" ]; then
		t_energy=$(echo "scale=5; $t_energy+$val/$iteration_float" | bc)
		echo -ne "total_energy: $val " >> $output_file
		if [ "$r" == "1" ]; then
			x=$totalnodes
		elif [ "$r" == "2" ]; then
			x=$totalflow
		elif [ "$r" == "3" ]; then
			x=$packet_rate
		elif [ "$r" == "4" ]; then
			x=$speed
		fi
		echo -ne "$x $val\n" >> $energy_file
	elif [ "$l" == "10" ]; then
		energy_bit=$(echo "scale=5; $energy_bit+$val/$iteration_float" | bc)
		echo -ne "energy_per_bit: $val " >> $output_file
	elif [ "$l" == "11" ]; then
		energy_byte=$(echo "scale=5; $energy_byte+$val/$iteration_float" | bc)
		echo -ne "energy_per_byte: $val " >> $output_file
	elif [ "$l" == "12" ]; then
		energy_packet=$(echo "scale=5; $energy_packet+$val/$iteration_float" | bc)
		echo -ne "energy_per_packet: $val " >> $output_file
	elif [ "$l" == "13" ]; then
		total_retransmit=$(echo "scale=5; $total_retransmit+$val/$iteration_float" | bc)
		echo -ne "total_retrnsmit: $val \n" >> $output_file
	fi


	echo "$val"
done < tcp_wireless.out



i=$(($i+1))
l=0
#################END AN ITERATION
done

#dir="/home/ubuntu/ns2_data/iTCP/"
#dir=""
under="_"
output_file="$output_file_format$under$r$under$r.out"

echo -ne "Throughput:          $thr " > $output_file
echo -ne "AverageDelay:         $del " >> $output_file
echo -ne "Sent Packets:         $s_packet " >> $output_file
echo -ne "Received Packets:         $r_packet " >> $output_file
echo -ne "Dropped Packets:         $d_packet " >> $output_file
echo -ne "PacketDeliveryRatio:      $del_ratio " >> $output_file
echo -ne "PacketDropRatio:      $dr_ratio " >> $output_file
echo -ne "Total time:  $time " >> $output_file
echo -ne "Total energy consumption:        $t_energy " >> $output_file
echo -ne "Average Energy per bit:         $energy_bit " >> $output_file
echo -ne "Average Energy per byte:         $energy_byte " >> $output_file
echo -ne "Average energy per packet:         $energy_packet " >> $output_file
echo "total_retransmit:         $total_retransmit " >> $output_file

if [ "$r" == "1" ]; then
			x="Nodes"
elif [ "$r" == "2" ]; then
			x="Flow"
elif [ "$r" == "3" ]; then
			x="Packets"
elif [ "$r" == "4" ]; then
			x="Speed"
fi

gnuplot -persist <<-EOFMarker
set title '$x Vs Energy'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$energy_file' using 1:2 with linespoints

EOFMarker

gnuplot -persist <<-EOFMarker
set title '$x Vs Throughput'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$thr_file' using 1:2 with linespoints

EOFMarker

gnuplot -persist <<-EOFMarker
set title '$x Vs Delay'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$delay_file' using 1:2 with linespoints

EOFMarker

gnuplot -persist <<-EOFMarker
set title '$x Vs Drop Ratio'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$dropratio_file' using 1:2 with linespoints

EOFMarker

gnuplot -persist <<-EOFMarker
set title '$x Vs Delivery ratio'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$deliveryratio_file' using 1:2 with linespoints

EOFMarker

r=$(($r+1))
#######################################END A ROUND
done

mv *.out out_folder/
