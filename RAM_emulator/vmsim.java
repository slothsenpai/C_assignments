import java.util.*;
import java.io.*;

public class vmsim{

	public static void main(String[] args){
	
		int numFrames = -1;
		String alg = "";
		int refreshRate = 250; // If not passed as an arg, this will be the assumed refresh rate
		Scanner tracer = new Scanner(System.in);
		String traceFile = "";
		long pageFaults = 0;
		long diskWrites = 0;
		long memAccesses = 0;
		

		if(args.length >= 5){
			if(args[0].equals("-n")){ // Checks that the first arg is -n, if not print an error message then system exit
				try{
					numFrames = Integer.parseInt(args[1]); // tries to convert arg following -n to int, system exit upon failure
				} catch(NumberFormatException nfe){
					System.out.println("Expected a number after -n");
					System.exit(1);
				}
			}
			else{
				System.out.println("Invalid Args\nExpected -n <numframes> -a <opt|clock|nru|rand> [-r <refresh>] <tracefile>");
				System.exit(1);
			}
			
			if(args[2].equals("-a")){ // Checks that third arg is -a, if not print error message then system exit
				alg = args[3];
				alg = alg.toLowerCase(); // store arg following -a and convert to lowercase for consistency
				
				// If the arg doesn't match any of our algorithms, print an error message then system exit
				if(!alg.equals("opt") && !alg.equals("clock") && !alg.equals("nru") && !alg.equals("rand")){
					System.out.println("Unsupported argument after -a\nExpected opt, clock, nru, or rand");
					System.exit(1);
				}
				
				
			}
			else{
				System.out.println("Invalid Args\nExpected -n <numframes> -a <opt|clock|nru|rand> [-r <refresh>] <tracefile>");
				System.exit(1);
			}
			
			
			
			// If the number of args is exactly five, we assume the fifth argument is the trace file and try to open it
			// If the file isn't found or cannot be opened, print an error message then system exit
			if(args.length == 5){
				try{
					traceFile = args[4];
					File trace = new File(args[4]);
				 	tracer = new Scanner(trace);
					
				} catch(IOException ioe){
					System.out.println("Couldn't find file");
					System.exit(1);
				}
			}
			
			// If the number of args is exactly seven, we assume the fifth argument is that of the refresh rate flag
			// We then try to parse the sixth arg as an int representing the refresh rate, but if either of these tests fail
			// we print an error message and system exit
			else if(args.length == 7){
				if(args[4].equals("-r")){
					try{
						refreshRate = Integer.parseInt(args[5]);
					} catch(NumberFormatException nfe){
						System.out.println("Expected a number after -r");
						System.exit(1);
					}
					if(refreshRate <= 0){
						System.out.println("Enter a refresh rate greater than zero");
						System.exit(1);
					}
				}
				else{
					System.out.println("Invalid Arguments\nExpected -n <numframes> -a <opt|clock|nru|rand> [-r <refresh>] <tracefile>");
					System.exit(1);
				}
				
				// Like above, we assume the last arg is that of the trace file and try to open it. 
				// If we fail, print an error message then system exit
				try{
					traceFile = args[6];
					File trace = new File(args[6]);
				 	tracer = new Scanner(trace);
				} catch(IOException ioe){
					System.out.println("Couldn't find file");
					System.exit(1);
				}
			}
		}
		else{
			System.out.println("Invalid Number of Arguments\nExpected -n <numframes> -a <opt|clock|nru|rand> [-r <refresh>] <tracefile>");
			System.exit(1);
		}
		

		
		int[] RAM = new int[numFrames]; // int array which will act as our RAM
		Arrays.fill(RAM, -1);
		int ramIndex = 0; // Index which will be used to make the first few memory accesses a little faster
		PageTable pTable = new PageTable();
		
		if(alg.equals("opt")){
			RefList[] refs = new RefList[0xFFFFF]; // Given the page number will never exceed 2^20, I chooses 0xFFFFF as the size of the reference array
			long lineNum = 0;
			
			/*
				Preprocessing the trace file below
				Read in each line and parse out the page number, then add the line on which that page number is referenced to the reference array
				The reference array is indexed by page number
			*/
			
			while(tracer.hasNext()){ 
				String[] addr;
				String line = tracer.nextLine();
				addr = line.split(" ");
				long address = Long.parseLong(addr[0], 16);
				int pageNo = (int)(address >> 12);
				if(refs[pageNo] == null){
					refs[pageNo] = new RefList();
				}
				refs[pageNo].add(lineNum);
				lineNum++;
			}
			tracer.close(); // close the scanner used to preprocess the trace file so I can open another Scanner to the trace file
			
			Scanner nextPass = new Scanner(System.in);
			try{
				nextPass = new Scanner(new File(traceFile));
			}
			catch(IOException ioe){
				System.out.println("Couldn't find file");
				System.exit(1);
			}
			
			/*
				Now do the actual work of the opt algorithm
				read in each line of the trace file, parse out the page number, and try to add it to RAM
				if something needs to be evicted, look at the pages in RAM and see which one will be referenced farthest in the future
				whichever process is referenced farthest in the future gets evicted
			*/
			while(nextPass.hasNext()){
				String[] addr;
				String line = nextPass.nextLine();
				addr = line.split(" ");
				char action = addr[1].charAt(0);
				long address = Long.parseLong(addr[0], 16);
				int pageNo = (int)(address >> 12);
				int offset = (int)(address & 0xFFF);
				boolean hit = false;
				for(int i = 0; i < numFrames; i++){
					if(RAM[i] == pageNo){
						hit = true;
					}
				}
				if(hit){
					System.out.println("hit");
					if(action == 'W'){
						pTable.writeTo(pageNo);
					}
					refs[pageNo].pop();
				}
				else{
					if(ramIndex <= numFrames-1){
						pageFaults++;
						memAccesses++;
						RAM[ramIndex] = pageNo;
						pTable.addToRAM(pageNo, ramIndex);
						refs[pageNo].pop();
						if(action == 'W'){
							pTable.writeTo(pageNo);
						}
						ramIndex++;
						System.out.println("page fault - no eviction");
					}
					else{
						pageFaults++;
						memAccesses++;
						int pageToEvict = -1;
						long largest = -1;
						int evictIndex = -1;
						for(int i = 0; i < numFrames; i++){
							if(refs[RAM[i]].peek() > largest){
								largest = refs[RAM[i]].peek();
								pageToEvict = RAM[i];
								evictIndex = i;
							}
						}
						if(pTable.isDirty(pageToEvict)){
							diskWrites++;
							memAccesses++;
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							refs[pageNo].pop();
							System.out.println("page fault - evict dirty");
						}
						else{
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							refs[pageNo].pop();
							System.out.println("page fault - evict clean");
						}
					}
				}
			}
			
		}
		else if(alg.equals("clock")){
			int pointer = 0; // int which will serve as a pointer to the next page to be evicted should eviction be necessary
			
			/*
				Clock algorithm
				Again, read in each line of the trace file and parse out the page number
				If eviction is required, look at the page number in RAM pointed to by the pointer
				If this page has the reference bit set, unset the reference bit and move the pointer to the next index in RAM
					repeat the above until an unreferenced page is found, this is the page which will be evicted
			*/
			while(tracer.hasNext()){
				String[] addr;
				String line = tracer.nextLine();
				addr = line.split(" ");
				char action = addr[1].charAt(0);
				long address = Long.parseLong(addr[0], 16);
				int pageNo = (int)(address >> 12);
				int offset = (int)(address & 0xFFF);
				boolean hit = false;
				for(int i = 0; i < numFrames; i++){
					if(RAM[i] == pageNo){
						hit = true;
					}
				}
				if(hit){
					System.out.println("hit");
					if(action == 'W'){
						pTable.writeTo(pageNo);
					}
					pTable.ref(pageNo);
				}
				else{
					if(ramIndex <= numFrames-1){
						pageFaults++;
						memAccesses++;
						RAM[ramIndex] = pageNo;
						pTable.addToRAM(pageNo, ramIndex);
						if(action == 'W'){
							pTable.writeTo(pageNo);
						}
						ramIndex++;
						pointer = (pointer + 1) % numFrames;
						System.out.println("page fault - no eviction");
					}
					else{
						pageFaults++;
						memAccesses++;
						while(pTable.isRef(RAM[pointer])){
							pTable.deref(RAM[pointer]);
							pointer = (pointer + 1) % numFrames;
						}
						int evictIndex = pointer;
						int pageToEvict = RAM[evictIndex];
						if(pTable.isDirty(pageToEvict)){
							diskWrites++;
							memAccesses++;
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							pointer = (pointer + 1) % numFrames;
							System.out.println("page fault - evict dirty");
						}
						else{
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							pointer = (pointer + 1) % numFrames;
							System.out.println("page fault - evict clean");
						}
					}
				}
			}
		}
		else if(alg.equals("nru")){
			long refTime = 1; // int which will be used to keep track of "time"
			
			/*
				This algorithm behaves similar to the clock algorithm, but rather unsets all the reference bits of the pages currently in RAM
					after some set amount of time
				Should eviction be necessary, the algorithm prefers to evict pages in this order:
					unreferenced, clean pages
					unreferenced, dirty pages
					referenced, clean pages
					referenced, dirty pages
				the algorithm will cycle over the pages in RAM and select the first page which is most preferred among the pages currently in RAM, then evict that page
			*/
			while(tracer.hasNext()){
				String[] addr;
				String line = tracer.nextLine();
				addr = line.split(" ");
				char action = addr[1].charAt(0);
				long address = Long.parseLong(addr[0], 16);
				int pageNo = (int)(address >> 12);
				int offset = (int)(address & 0xFFF);
				boolean hit = false;
				for(int i = 0; i < numFrames; i++){
					if(RAM[i] == pageNo){
						hit = true;
					}
				}
				if(hit){
					System.out.println("hit");
					if(action == 'W'){
						pTable.writeTo(pageNo);
					}
				}
				else{
					if(ramIndex <= numFrames-1){
						pageFaults++;
						memAccesses++;
						RAM[ramIndex] = pageNo;
						pTable.addToRAM(pageNo, ramIndex);
						if(action == 'W'){
							pTable.writeTo(pageNo);
						}
						ramIndex++;
						System.out.println("page fault - no eviction");
					}
					else{
						pageFaults++;
						memAccesses++;
						int evictIndex = -1;
						int pageToEvict = -1;
						boolean best = false;
						boolean second = false;
						boolean third = false;
						for(int i = 0; i < numFrames; i++){
							if(!pTable.isRef(RAM[i]) && !pTable.isDirty(RAM[i])){
								best = true;
								evictIndex = i;
								pageToEvict = RAM[i];
							}
							else if(!best && !pTable.isRef(RAM[i]) && pTable.isDirty(RAM[i])){
								second = true;
								evictIndex = i;
								pageToEvict = RAM[i];
							}
							else if(!best &&!second && pTable.isRef(RAM[i]) && !pTable.isDirty(RAM[i])){
								third = true;
								evictIndex = i;
								pageToEvict = RAM[i];
							}
							else if(!best && !second && !third && pTable.isRef(RAM[i]) && pTable.isDirty(RAM[i])){
								evictIndex = i;
								pageToEvict = RAM[i];
							}
						}
						if(pTable.isDirty(pageToEvict)){
							diskWrites++;
							memAccesses++;
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							System.out.println("page fault - evict dirty");
						}
						else{
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							System.out.println("page fault - evict clean");
						}
					}
				}
				if(refTime == refreshRate){
					for(int i = 0; i < numFrames; i++){
						if(RAM[i] != -1){
							pTable.deref(RAM[i]);
						}
					}
					refTime = 0;
				}
				refTime++;
			}
		}
		
		/*
			The random algorithm doesn't pay attention to referenced or dirty bits, it simply evicts a page at random when eviction is necessary
		*/
		else if(alg.equals("rand")){
			Random random = new Random();
			while(tracer.hasNext()){
				String[] addr;
				String line = tracer.nextLine();
				addr = line.split(" ");
				char action = addr[1].charAt(0);
				long address = Long.parseLong(addr[0], 16);
				int pageNo = (int)(address >> 12);
				int offset = (int)(address & 0xFFF);
				boolean hit = false;
				for(int i = 0; i < numFrames; i++){
					if(RAM[i] == pageNo){
						hit = true;
					}
				}
				if(hit){
					System.out.println("hit");
					if(action == 'W'){
						pTable.writeTo(pageNo);
					}
				}
				else{
					if(ramIndex <= numFrames-1){
						pageFaults++;
						memAccesses++;
						RAM[ramIndex] = pageNo;
						pTable.addToRAM(pageNo, ramIndex);
						if(action == 'W'){
							pTable.writeTo(pageNo);
						}
						ramIndex++;
						System.out.println("page fault - no eviction");
					}
					else{
						pageFaults++;
						memAccesses++;
						int evictIndex = random.nextInt(numFrames);
						int pageToEvict = RAM[evictIndex];
						if(pTable.isDirty(pageToEvict)){
							diskWrites++;
							memAccesses++;
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							System.out.println("page fault - evict dirty");
						}
						else{
							pTable.evict(pageToEvict);
							RAM[evictIndex] = pageNo;
							pTable.addToRAM(pageNo, evictIndex);
							if(action == 'W'){
								pTable.writeTo(pageNo);
							}
							System.out.println("page fault - evict clean");
						}
					}
				}
			}
		}
		if(alg.equals("opt")){
			System.out.println("Algorithm: Optimal");
		}
		else if(alg.equals("clock")){
			System.out.println("Algorithm: Clock");
		}
		else if(alg.equals("nru")){
			System.out.println("Algorithm: NRU");
		}
		else if(alg.equals("rand")){
			System.out.println("Algorithm: Random");
		}
		
		System.out.printf("Number of Frames:\t%d%n", numFrames);
		System.out.printf("Total Memory Accesses:\t%d%n", memAccesses);
		System.out.printf("Total Page Faults:\t%d%n", pageFaults);
		System.out.printf("Total writes to disk:\t%d%n", diskWrites);
	}

	
	
	
	
	
	
}
