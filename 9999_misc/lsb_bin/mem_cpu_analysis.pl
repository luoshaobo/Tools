#! /usr/bin/perl

###########################################
#global variables from command line options
###########################################
use Getopt::Long;
#file names
$infilename 		= "";
$readIn				= "";

GetOptions(
	"in=s"		=> 	\$infilename,
);

if($infilename eq "")
{
    printf ("usage example:\nperl test.pl --in=infilename.log\n"); 
    printf ("ERROR: input file name missing\n");
    exit; 
}

if(!open(INFILE, $infilename))
{
  printf ("ERROR: cannot open input file $infilename for reading");
  exit;
}

if(!open(CPUOUTFILE,">",$infilename.'_CPU.csv'))
{
  printf ("ERROR: cannot open cpu output file for writing");
  exit;
}

if(!open(TOPCPUOUTFILE,">",$infilename.'_TopLoad.csv'))
{
  printf ("ERROR: cannot open top cpu load output file for writing");
  exit;
}

if(!open(MEMOUTFILE,">",$infilename.'_MEM.csv'))
{
  printf ("ERROR: cannot open mem output file for writing");
  exit;
}

if(!open(TOPMEMOUTFILE,">",$infilename.'_TopMem.csv'))
{
  printf ("ERROR: cannot open top mem output file for writing");
  exit;
}
	
while(<INFILE>)
{
	# get the line
	$readIn = $_;
	
	# print $line;
	if($readIn =~ "GlobalMemory")
	{
		parserGlobalMemory();
	}
	elsif($readIn =~ "GlobalLoad" )
	{
		parserGlobalCpuLoading();
	}
	elsif($readIn =~ "TopThreadLoad" )
	{
		parserTopThread();
	}
	elsif($readIn =~ "TopMemoryUsage")
	{
		parerTopMemory();
	}
}

sub parserGlobalMemory
{
		if($readIn =~ "(<used memory>/<free memory>/<total>)")
		{
			#print $readIn;
		}
		else
		{
			# format output string and get time stamp
			my @fields1 = split /~/, $readIn;
			
			#0: timestamp
			#1: used memory
			#2: free memory
			#3: total
			
			# split global memory output, 
			my @fields2 = split /:/, $fields1[5];
			
			# remove tail
			my @fields3 = split / /,@fields2[2];
			
			# split memory print
			my @fields4 = split /\//, @fields3[0];
			
			# parser global memory format
			my $usedmem = substr(@fields4[0],1);
			my $freemem = @fields4[1];		
			
			my $len = length(@fields4[2]);			
			my $totalmem = substr(@fields4[2],0, $len-1);
			
			print MEMOUTFILE $fields1[0],',',$usedmem,',',$freemem,',',$totalmem,",\r";
		}
}

sub parserGlobalCpuLoading()
{
		if($readIn =~ "(<Kernel>/<User>/<Total>)")
		{
			#print $readIn;
		}
		else
		{
			# format output string and get time stamp
			my @fields1 = split /~/, $readIn;
			
			#0: timestamp
			#1: kernel
			#2: user
			#3: total
						
			# split global load output, 
			my @fields2 = split /\(/, $readIn;
			
			# remove tail
			my @fields3 = split / /,@fields2[2];
			
			# split load print
			my @fields4 = split /\//, @fields3[0];
			
			# parser global memory format
			my $kernelLoad = substr(@fields4[0],0);
			my $userLoad = @fields4[1];		
			
			my $len = length(@fields4[2]);			
			my $totalLoad = substr(@fields4[2],0, $len-1);
			
			print CPUOUTFILE $fields1[0],',',$kernelLoad,',',$userLoad,',',$totalLoad,",\r";
		}
}

sub parerTopMemory()
{
		if($readIn =~ "(<RC>/<RW>/<Stack>/<Total>)")
		{
			#print $readIn;
		}
		else
		{
			# format output string and get time stamp
			my @fields1 = split /~/, $readIn;
			
			#print $readIn;
			
			#0: timestamp
			#1: PID Name
			#2: PID
			#3: RC
			#4: RW
			#5: Stack
			#6: Total
			
			# split top mem output, 
			my @fields2 = split / /, $readIn;
			
			my $procName = @fields2[2];
			my $procID = @fields2[3];
			my $procMem = @fields2[4];
			
			my @fields3 = split /\//, $procMem;
			
			my $rcMem = substr(@fields3[0],1);
			my $rwMem = @fields3[1];
			my $stackMem = @fields3[2];
			
			my $len = length(@fields3[3]);	
			my $totalMem = substr(@fields3[3],0, $len-1);
						
			print TOPMEMOUTFILE $fields1[0],',',$procName,',',$procID,',',$rcMem,',',$rwMem,',',$stackMem,',',$totalMem,",\r";
		}
}

sub parserTopThread()
{
		if($readIn =~ "(<Kernel>/<User>)@<Prio>")
		{
			#print $readIn;
		}
		else
		{
			# format output string and get time stamp
			my @fields1 = split /~/, $readIn;
			
			#print @fields1[5];
			
			#0: timestamp
			#1: PID Name
			#2: PID
			#3: Kernel
			#4: User
			
			# split top mem output, 
			my @fields2 = split / /, @fields1[5];
			
			#print @fields2;

			my $procID = @fields2[2];
			my $procName = @fields2[3];
			my $threadID = @fields2[4];
			my $threadLoad = @fields2[5];
			
			my @fields3 = split /\//, $threadLoad;
			
			#print $threadLoad;
			
			my $kernelLoad = substr(@fields3[0],1);
			#print $kernelLoad;

			my @userLoad = split /\)/, @fields3[1];
			#print @userLoad[0];
			
			#isThreadAvailable();
						
			print TOPCPUOUTFILE $fields1[0],',',$procName,',',$procID,',',$threadID,',',$kernelLoad,',',@userLoad[0],",\r";
		}
}