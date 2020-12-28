rm *.dat
i=1;
end=4;

thr_file="sat_thr_data_$r.dat"
delay_file="sat_delay_data_$r.dat"
deliveryratio_file="sat_deliveryratio_data_$r.dat"

while [ $i -le $end ]
do
if [ "$i" == "1" ]; then
			echo "Number of satellites: 11"
elif [ "$i" == "2" ]; then
			echo "Number of satellites: 9"
elif [ "$i" == "3" ]; then
			echo "Number of satellites: 7"
elif [ "$i" == "4" ]; then
			echo "Number of satellites: 5"
fi
ns sat.tcl $i;
awk -f sat.awk sat.tr > sat.out


while read val
do
l=$(($l+1))

if [ "$l" == "1" ]; then
		if [ "$i" == "1" ]; then
			x=11
		elif [ "$i" == "2" ]; then
			x=9
		elif [ "$i" == "3" ]; then
			x=7
		elif [ "$i" == "4" ]; then
			x=5
		fi
		echo -ne "$x $val\n" >> $thr_file
		echo "Throughput: $val"
	elif [ "$l" == "2" ]; then
		if [ "$i" == "1" ]; then
			x=11
		elif [ "$i" == "2" ]; then
			x=9
		elif [ "$i" == "3" ]; then
			x=7
		elif [ "$i" == "4" ]; then
			x=5
		fi
		echo -ne "$x $val\n" >> $deliveryratio_file
		echo "Delivery ratio: $val"
	elif [ "$l" == "3" ]; then
		if [ "$i" == "1" ]; then
			x=11
		elif [ "$i" == "2" ]; then
			x=9
		elif [ "$i" == "3" ]; then
			x=7
		elif [ "$i" == "4" ]; then
			x=5
		fi
		echo -ne "$x $val\n" >> $delay_file
		echo "Drop Ratio: $val"

	fi

done < sat.out


i=$(($i+1))
l=0;
done

gnuplot -persist <<-EOFMarker
set title 'Satellite_number Vs Energy'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$thr_file' using 1:2 with linespoints

EOFMarker

gnuplot -persist <<-EOFMarker
set title 'Satellite_number Vs Energy'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$delay_file' using 1:2 with linespoints

EOFMarker


gnuplot -persist <<-EOFMarker
set title 'Satellite_number Vs Energy'
set style line 1 \
linecolor rgb '#0060ad' \
linetype 1 linewidth 2 \
pointtype 7 pointsize 1.5

plot '$deliveryratio_file' using 1:2 with linespoints

EOFMarker




