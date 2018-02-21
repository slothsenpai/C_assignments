#include "graphics.h"
#include "library.c"

int main(){
	char hold;
	int i = 0;
	int j = 0;
	int n1 = 1;
	int n2 = 1;
	int n;
	color_t b = RGB(0, 0, 0);
	color_t w = RGB(31, 63, 31);
	color_t r = (color_t)0xA125;
	color_t o = (color_t)0xE363;
	color_t f = (color_t)0xEDF4;
	color_t guess_who[] = {
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, b, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, o, b, w, w, w, b, b, w, w, w, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, o, o, b, w, w, b, o, b, w, w, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, o, o, b, w, w, b, o, b, b, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, o, o, o, b, w, w, b, o, o, b, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, o, o, o, o, b, w, b, o, o, o, b, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, b, b, b, b, b, b, b, b, b, w, b, o, o, o, o, b, w, b, o, o, o, b, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, b, o, o, o, o, o, o, o, o, o, b, b, o, o, o, o, o, b, b, o, o, o, o, b, w, w, w, w,
		w, w, w, w, w, w, w, w, w, b, b, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, b, o, o, o, o, b, b, w, w, w,
		w, w, w, b, b, w, w, w, b, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, b, w, w, w,
		w, w, w, b, o, b, b, b, b, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, b, w, w, w,
		w, w, w, w, b, o, o, o, b, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, b, w, w, w,
		w, w, w, w, b, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, b, w, w, w,
		w, w, b, b, b, o, o, o, o, o, o, o, o, b, b, b, b, b, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, b, w, w, w,
		w, b, o, o, o, o, o, o, o, o, o, o, o, b, b, f, f, f, b, b, b, b, b, o, o, o, o, o, o, o, o, o, o, o, o, b, w, w, w,
		b, b, b, o, o, o, o, o, o, o, o, o, o, o, o, b, f, f, f, f, f, f, f, b, b, b, b, b, o, o, o, o, o, o, b, w, w, w, w,
		w, w, w, b, b, b, o, o, o, o, o, o, b, b, o, o, b, f, f, f, f, f, f, f, f, f, f, f, b, b, b, b, b, b, w, w, w, w, w,
		w, w, w, w, b, o, o, o, o, o, o, o, b, f, b, b, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w, w, w, w, w,
		w, w, w, w, b, o, o, o, o, o, o, o, o, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w, w, w, w,
		w, w, w, b, o, o, o, o, o, o, o, o, o, o, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w, w, w,
		w, w, w, b, o, o, o, o, o, o, o, o, o, o, o, b, f, f, f, f, f, f, f, f, f, f, b, b, f, f, f, f, b, b, b, w, w, w, w,
		w, w, w, b, o, o, o, o, o, o, o, o, o, o, o, b, f, f, f, f, f, f, f, f, f, f, f, b, f, f, f, f, f, f, f, b, w, w, w,
		w, w, w, b, o, o, o, o, o, o, o, o, o, o, o, b, f, f, b, f, f, b, b, b, b, b, b, f, b, b, b, b, b, b, b, b, w, w, w,
		w, w, w, b, o, o, o, o, o, o, o, o, o, o, o, o, b, f, b, b, b, w, w, w, b, w, w, b, b, w, w, w, w, b, w, b, w, w, w,
		w, w, w, b, o, o, o, o, o, o, o, o, o, o, o, o, b, f, b, w, w, b, b, b, b, b, b, b, b, b, b, b, b, b, b, b, w, w, w,
		w, w, w, w, b, o, o, o, o, o, o, o, b, b, o, o, b, f, b, b, b, f, f, f, f, f, f, f, b, b, b, f, f, f, f, b, w, w, w,
		w, w, w, w, b, o, o, o, o, o, o, b, f, f, b, o, b, f, f, b, f, f, f, f, f, f, f, b, f, f, b, b, b, b, b, w, w, w, w,
		w, w, w, w, w, b, b, o, o, o, b, f, b, b, f, b, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w,
		w, w, w, w, w, w, w, b, b, o, b, f, b, f, b, f, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w,
		w, w, w, w, w, w, w, w, w, b, b, f, f, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, b, w, w,
		w, w, w, w, w, w, w, w, w, w, w, b, f, f, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, b, b, b, b, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, b, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, b,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, f, f, f, f, f, f, f, f, f, f, b, b, b, b, b, b, b, b, b, b, b, b, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, w, b, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, w, b, r, b, f, f, f, f, f, f, f, f, f, f, f, f, f, f, b, b, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, w, b, r, r, b, f, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, b, r, r, b, b, f, f, f, f, f, f, f, f, f, f, f, f, b, w, w, w, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, w, w, b, r, r, b, b, f, f, f, f, f, f, f, f, f, f, f, f, b, b, w, w, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, w, b, b, b, r, r, r, b, f, f, f, f, f, f, f, f, f, f, f, b, r, r, b, w, w, w, w, w, w, w, w,
		w, w, w, w, w, w, w, w, b, r, b, r, r, r, r, b, f, f, f, f, f, f, f, f, f, f, b, r, r, r, r, b, w, w, w, w, w, w, w,
		w, w, w, w, w, w, b, b, r, r, b, r, r, r, r, b, b, b, b, f, f, f, f, f, f, f, b, b, r, r, r, b, w, w, w, w, w, w, w,
		w, w, w, w, w, b, r, r, r, r, b, r, r, r, r, b, w, w, w, b, b, b, b, b, b, b, b, w, b, r, r, b, w, w, w, w, w, w, w,
		w, w, w, w, b, r, r, r, r, r, b, r, r, r, r, r, b, w, w, w, w, w, w, w, w, w, w, w, b, r, r, r, b, b, w, w, w, w, w,
		w, w, w, b, r, r, r, r, r, r, b, r, r, r, r, r, b, w, w, w, w, w, w, w, w, w, w, w, b, r, r, r, b, r, b, w, w, w, w,
		w, w, b, r, r, r, r, r, r, r, b, r, r, r, r, r, r, b, w, w, w, w, w, w, w, w, w, w, b, r, r, r, b, r, b, w, w, w, w,
		w, b, r, r, r, r, r, r, r, r, b, r, r, r, r, r, r, b, w, w, w, w, w, w, w, w, w, w, b, r, r, r, b, r, r, b, w, w, w
		}; //bitmap entered by hand, worth it
	
	init_graphics();
	char *t_buf; // char pointer which will point to our offscreen buffer
	t_buf = (char *)new_offscreen_buffer();
	while(hold != 'q'){

		hold = getkey(); // gets keys as they are pressed
		
		if(hold == 'q'){
			break;
		}
		else if(hold == '+'){
			// draws random pixel colors to the offscreen buffer, then blits them to the framebuffer
			// I had the n's follow the fibonacci sequence because it ends up drawing cool shapes and the colors vary enough to be interesting
			n = n1 + n2;
			
			for(i = 0; i < 479; i++){
				for(j = 0; j < 639; j++){
					draw_pixel(t_buf, j, i, RGB(j+n1*i, i*(n2+j), n1*n2+n));
				}
			}
			blit(t_buf);
			n1 = n2;
			n2 = n;
			
		}
		// draws a checkerboard using the draw_square function (which utilizes the draw_line function)
		else if(hold == 'c'){
			int counter = 0;
			color_t c;
			for(i = 0; i < 105; i++){
				for(j = 0; j < 105; j++){
					if(counter % 2 == 0){
						c = RGB(31, 63, 31);
					}
					else{
						c = RGB(0, 0, 0);
					}
					draw_square(t_buf, j*5, i*5, c);
					counter++;
				}
			}
			blit(t_buf);
			sleep_ms(500);
			clear_screen(buf_map);
		}
		// draws pixel art using the draw_square function (which utilizes the draw_line function)
		else if(hold == 'f'){
			int x = 100;
			int y = 100;
			int counter = 0;
			clear_screen(t_buf);
			color_t c;
			for(i = 0; i < 1950; i++){
				draw_square(t_buf, x+(i%39)*5, y+(i/39)*5, guess_who[i]);
			}
			blit(t_buf);
			sleep_ms(1200);
			clear_screen(buf_map);
		}
		
		else{
			int midx = 639/2;
			int maxx = 638;
			int midy = 479/2;
			int maxy = 478;
			clear_screen(t_buf);
			blit(t_buf);
			// sets the screen to white
			for(i = 0; i < 479; i++){
				for(j = 0; j < 639; j++){
					draw_pixel(t_buf, j, i, RGB(31, 63, 31));
				}
			}
			// draws a series of concentric diamonds which demonstrates the draw_line functionality
			for(i = 0; i < 125; i++){
				draw_line(t_buf, midx, 0+(4*i), maxx-(4*i), midy, r);
				blit(t_buf);
				draw_line(t_buf, maxx-(4*i), midy, midx, maxy-(4*i), r);
				blit(t_buf);
				draw_line(t_buf, midx, maxy-(4*i), 0+(4*i), midy, r);
				blit(t_buf);
				draw_line(t_buf, 0+(4*i), midy, midx, 0+(4*i), r);
				blit(t_buf);
			}
		}
		
		
		
		
	}
	
	exit_graphics();
	return 0;

}
