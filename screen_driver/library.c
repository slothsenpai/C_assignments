#include "graphics.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int buf_fd; // int used to represent a file descriptor for the frame buffer
char *buf_map; // void pointer which will receive the address of the frame buffer mapping
int buf_size; // int which will hold the size in bytes of the framebuffer
struct fb_var_screeninfo vinfo; // struct to be used to get screen info
struct fb_fix_screeninfo finfo; // struct to be used to get screen info



void init_graphics(){
	
	char clear[] = "\033[2J"; // ANSI escape code to clear the terminal
	char hide_cursor[] = "\033[?25l"; // ANSI escape code to hide the cursor
	struct termios term; // struct which will contain info about the terminal
	
	buf_fd = open("/dev/fb0", O_RDWR); // Opens framebuffer 0 for reading and writing
	if(buf_fd == -1){
		perror("open buffer");
	}
	
	int result = ioctl(buf_fd, FBIOGET_VSCREENINFO, &vinfo); // populates the vinfo struct with info about the framebuffer
	if(result == -1){
		perror("ioctl VSREENINFO");
	}
	result = ioctl(buf_fd, FBIOGET_FSCREENINFO, &finfo); // populates the finfo struct with info about the framebuffer
	if(result == -1){
		perror("ioctl FSCREENINFO");
	}
	
	buf_size = vinfo.yres_virtual*finfo.line_length; // calculate the size of the frame buffer
	
	buf_map = (char *)mmap(0, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0); // maps the framebuffer to RAM with reading and writing capabilites
	if(buf_map == MAP_FAILED){
		perror("mapping");
	}
	
	write(0, hide_cursor, sizeof(hide_cursor)); // writes the ANSI escape code to hide the cursor to the terminal
	write(0, clear, sizeof(clear)); // writes the ANSI escape code to clear the terminal to the terminal
	
	ioctl(0, TCGETS, &term); // populates term struct with info about the terminal
	
	term.c_lflag &= ~ICANON; // unsets the ICANON bit to disable cannonical node
	term.c_lflag &= ~ECHO; // unsets the ECHO bit to disable character echoing in the terminal
	
	ioctl(0, TCSETS, &term); // applies changes to terminal settings
}

void exit_graphics(){
	struct termios term; // struct which will contain info about the terminal
	char clear[] = "\033[2J"; // ANSI escape code to clear the terminal
	char show_cursor[] = "\033[?25h"; // ANSI escape code to enable the cursor
	
	close(buf_fd); // closes the file descriptor pointing to the framebuffer 0
	
	munmap(buf_map, buf_size); // unmaps the framebuffer from RAM
	
	ioctl(0, TCGETS, &term); // populates term struct with info about the terminal
	
	term.c_lflag |= ~ICANON; // sets the ICANON bit to enable cannonical mode
	term.c_lflag |= ~ECHO; // sets the ECHO bit to enable character echoing
	
	ioctl(0, TCSETS, &term); // applies the changes to the terminal settings
	
	write(0, clear, sizeof(clear)); // writes the ANSI escape code to clear the terminal to the terminal
	write(0, show_cursor, sizeof(show_cursor)); // writes the ANSI escape code to enable the cursor to the terminal
}

char getkey(){
	int keyboard_fd; // int which will hold the keyboard file descriptor
	int fd_catcher; // int which will be used to determine which file is ready to be read
	char the_char; // char which will eventually hold the char read from the keyboard
	struct timeval time; // struct which contains time info
	time.tv_sec = 0;
	time.tv_usec = 0;
	fd_set sets; // set of file descriptors required by the select() function, will only hold the file descriptor of the keyboard though
	keyboard_fd = open("/dev/tty", O_RDONLY); // opens the file descriptor of the keyboard
	if(keyboard_fd == -1){
		perror("open");
	}
	FD_SET(keyboard_fd, &sets); // adds the keyboard file descriptor to the set of file descriptors
	
	fd_catcher = select(keyboard_fd, &sets, NULL, NULL, &time); // call to select to see if the keyboard is ready to be read, if so returns the file descriptor of the keyboard
	if(fd_catcher == -1){
		perror("select");
	}
	int err = read(fd_catcher, &the_char, 1); // just in case, err is here to catch an error should it occur, otherwise the read was successful and we return the char
	if(err == -1){
		perror("read");
	}
	return the_char;
}

// "sleeps" the process for the numer of milliseconds described by ms
void sleep_ms(long ms){
	
	struct timespec time; // struct which will hold time info
	if(ms > 999){ // if the time to wait is longer than one second, break that time into seconds and remaining milliseconds
		time.tv_sec = ms/1000;
		time.tv_nsec = (ms%1000)*1000000;
	}
	else{ // otherwise convert from nanosecond to millisecond
		time.tv_sec = 0;
		time.tv_nsec = ms * 1000000;
	}
	int result; // int to catch errors should they occur
	result = nanosleep(&time, NULL);
	if(result  == -1){
		perror("nanosleep");
	}
}

// function loops over every pixel and sets it to black
void clear_screen(void *img){
	int x = 0;
	int y = 0;
	for(y = 0; y < 479; y++){
		for(x = 0; x < 639; x++){
			int xy = x * vinfo.bits_per_pixel/8 + y * finfo.line_length;
			*((color_t *)(img + xy)) = RGB(0, 0, 0);
		}
	}
}

// draws a pixel with the specified RGB value at the x and y coordinate described
void draw_pixel(void *img, int x, int y, color_t color){
	// Honestly, I didn't come up with this math myself, I saw a similar implementation after trying to figure this out on my own
	// the math below ended up working
	// source: https://gist.githubusercontent.com/FredEckert/3425429/raw/306e2ad9e2cbc8877df34393b6e8e3ce2dc83fb1/framebuffer.c
	if(x >= 0 && x <= 639 && y >= 0 && y <= 479){
		int xy = x * vinfo.bits_per_pixel/8 + y * finfo.line_length; 
		*((color_t *)(img + xy)) = color;
	}
	else{}
}

// Used Bresenham's Algorithm detailed here https://stackoverflow.com/questions/10060046/drawing-lines-with-bresenhams-line-algorithm
// I could not get other implementations working on my own as they couldn't handle negative slopes or situations where y2 < y1 or x2 < x1
// thus I used the implementation below as is. It does used integer math though!
void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c){
	if(x1 >= 0 && x1 <= 639 && x2 >= 0 && x2 <= 639 && y1 >= 0 && y1 <= 479 && y2 >= 0 && y2 <= 479){
		
		 int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
		 dx=x2-x1;
		 dy=y2-y1;
		 if(dx > 0){dx1 = dx;}
		 else{dx1 = dx/-1;}
		 if(dy > 0){dy1 = dy;}
		 else{dy1 = dy/-1;}
		 px=2*dy1-dx1;
		 py=2*dx1-dy1;
		 if(dy1<=dx1)
		 {
		  if(dx>=0)
		  {
		   x=x1;
		   y=y1;
		   xe=x2;
		  }
		  else
		  {
		   x=x2;
		   y=y2;
		   xe=x1;
		  }
		  draw_pixel(img, x, y, c);
		  for(i=0;x<xe;i++)
		  {
		   x=x+1;
		   if(px<0)
		   {
			px=px+2*dy1;
		   }
		   else
		   {
			if((dx<0 && dy<0) || (dx>0 && dy>0))
			{
			 y=y+1;
			}
			else
			{
			 y=y-1;
			}
			px=px+2*(dy1-dx1);
		   }
		   draw_pixel(img, x, y, c);
		  }
		 }
		 else
		 {
		  if(dy>=0)
		  {
		   x=x1;
		   y=y1;
		   ye=y2;
		  }
		  else
		  {
		   x=x2;
		   y=y2;
		   ye=y1;
		  }
		  draw_pixel(img, x, y, c);
		  for(i=0;y<ye;i++)
		  {
		   y=y+1;
		   if(py<=0)
		   {
			py=py+2*dx1;
		   }
		   else
		   {
			if((dx<0 && dy<0) || (dx>0 && dy>0))
			{
			 x=x+1;
			}
			else
			{
			 x=x-1;
			}
			py=py+2*(dx1-dy1);
		   }
		   draw_pixel(img, x, y, c);
		  }
		 }
		}
	else{}
}


// extra function I added for fun, draws a 5 pixel by 5 pixel square starting at the x,y coordinates provided utilizing the draw_line function
void draw_square(void *img, int x, int y, color_t color){
	if(x >= 0 && x <= 629 && y >= 0 && y <= 469){
		int x2 = x+5;
		int i = 0;
		for(i = 0; i < 5; i++){
			draw_line(img, x, y+i, x2, y+i, color);
		}
		
	}
}

// creates a new offscreen buffer and returns it
void *new_offscreen_buffer(){
	void *off_buf;
	
	off_buf = mmap(0, buf_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if(off_buf == MAP_FAILED){
		perror("buffer");
	}
	return off_buf;
}

// copies the offscreen buffer pointed to by src into the framebuffer, pixel by pixel
void blit(void *src){
	int x = 0;
	int y = 0;
	color_t c;
	for(y = 0; y < 479; y++){
		for(x = 0; x < 639; x++){
			int xy = x * vinfo.bits_per_pixel/8 + y * finfo.line_length;
			c = *((color_t *)(src + xy));
			*((color_t *)(buf_map + xy)) = c;
		}
	}
}