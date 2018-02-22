import java.util.*;

// Essentially a wrapper class for an ArrayList as I was having issues with creating an array of ArrayLists
// Rather than keep a linked list of references when running the opt algorithm, I decided to use an ArrayList and treat it like a queue

public class RefList{
	ArrayList<Long> references;
	
	public RefList(){
		references = new ArrayList<Long>();
	}
	
	public void add(long l){
		references.add(l);
	}
	
	public long peek(){ // looks at the first long in the queue, but doesn't remove it
		long ret;
		if(references.isEmpty()){
			ret = Long.MAX_VALUE;
		}
		else{
			ret = references.get(0);
		}
		return ret;
	}
	
	public void pop(){ // removes the first long from the queue
		if(!references.isEmpty()){
			references.remove(0);
		}
	}
	
	public boolean isEmpty(){
		return references.isEmpty();
	}
	
	public int size(){
		return references.size();
	}
	
}