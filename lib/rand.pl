

#$upper=492;
$upper=980;
$lower=20;
srand;
$lx=512;
$lz=512;
$ly=0;
$numnums=100;
for ($i=0; $i<$numnums; $i++)
{
	$x = int(rand( $upper-$lower+1 ) ) + $lower;
	$z = int(rand( $upper-$lower+1 ) ) + $lower;
	$y = int(rand( 360 ));
	while (sqrt(($z-$ls)*($z-$ls)+(($x-$lx)*($x-$lx)))<300)
	{
		$z=int(rand( $upper-$lower+1 ) ) + $lower;
	}
	while ($y-$ly>270)
	{
		$y = int(rand( 360 ));
		$ly = $y;
	}
	push(@ry,$y);
#	printf "x:%d z:%d\n",$x,$z;
	push(@rz,$z);
	push(@rx,$x);
	$lx=$x;
	$lz=$z;
}

printf "mx[$numnums]={";
for ($i=0;$i<$numnums;$i++)
{
	print $rx[$i],".0f,";
#	print "0.0f,";
	printf "\n" if ($i%10==0);
}
printf "};\n";
printf "mz[$numnums]={";
for ($i=0;$i<$numnums;$i++)
{
	print $rz[$i],".0f,";
#	print "0.0f,";
	printf "\n" if ($i%10==0);
}
printf "};\n";
printf "my[$numnums]={";
for ($i=0;$i<$numnums;$i++)
{
	if ($i%2==0)
	{
		print "0.0f,";
	} elsif ($i%3==0) {
		print -1*$ry[$i],".0f,";
	} else {
		print $ry[$i],".0f,";
	}
#	print "0.0f,";
	printf "\n" if ($i%10==0);
}
printf "};\n";
