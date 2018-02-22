import java.util.*;

public class PageTable{
	
	int[] table = new int[0xFFFFF]; // indexed by page number, keeps track of the frame number at which a page is loaded, -1 if not in RAM
	boolean[] dirty = new boolean[0xFFFFF]; // indexed by page number, acts as the dirty/modified bit for each page
	boolean[] reference = new boolean[0xFFFFF]; // indexed by page number, acts as the referenced bit for each page
	
	public PageTable(){
		Arrays.fill(table, -1);
	}
	
	public void addToRAM(int i, int frame){
		table[i] = frame;
		reference[i] = true;
	}
	
	public void ref(int i){
		reference[i] = true;
	}
	
	public void deref(int i){
		reference[i] = false;
	}
	
	public void evict(int i){
		table[i] = -1;
		reference[i] = false;
		dirty[i] = false;
	}
	
	public void writeTo(int i){
		dirty[i] = true;
	}
	
	public boolean isDirty(int i){
		return dirty[i];
	}
	
	public boolean isRef(int i){
		return reference[i];
	}
	
}