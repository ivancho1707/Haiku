/*

   How to use a custom non-system font.

 */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x8A, 0xB0, 0x0B, 0xD0, 0xA0, 0x27, 0x41, 0x5A, 0xAB, 0xBB, 0xA8, 0x7F, 0x03, 0xF0, 0x6C, 0x13 }
PBL_APP_INFO(MY_UUID, "Haiku", "Crayon Lion Studios", 1, 0 /* App version */, DEFAULT_MENU_ICON, APP_INFO_WATCH_FACE);


#define SCREENW 144
#define SCREENH 168
#define LANDS_W 144
#define LANDS_H 58
#define SMALL_W 20
#define SMALL_H 20
#define LARGE_W 35
#define LARGE_H 35
#define WEEKD_W 84
#define WEEKD_H 34


Window window;

BmpContainer img_morning, img_noon, img_night;

BmpContainer image_numbers_small[11];
// BmpContainer image_numbers_large[11];
BmpContainer image_week_days[7];

BmpContainer image_lbl_yr, image_lbl_mt, image_lbl_dy, image_lbl_hr, image_lbl_mn;

BmpContainer image_bg;

BmpContainer image_lbl_yr1, image_lbl_yr2, image_lbl_wd1, image_lbl_wd2;
BitmapLayer bml_year[3];

BitmapLayer bml_month[3];

BitmapLayer bml_day[3];

BitmapLayer bml_weekday;

BitmapLayer bml_hour[3];

BitmapLayer bml_minute[3];

int digit_used;
int digit_pos[3];

int current_year   = -1;
int current_month  = -1;
int current_wday   = -1;
int current_day    = -1;
int current_hour   = -1;
int current_minute = -1;

void calculate_digit_positions(int num/*, int *digit_pos */) {
	digit_used = 1;

	if (num < 11) {
		digit_pos[0] = num;
	} else if (num < 20) {
		digit_used = 2;
		digit_pos[0] = 10;
		digit_pos[1] = num % 10;
	} else {
		digit_pos[0] = num / 10;
		digit_pos[1] = 10;

		if (num % 10 == 0) {
			digit_used = 2;
		} else {
			digit_used = 3;
			digit_pos[2] = num % 10;
		}
	}
}

int calculate_12_format(int hr)
{
  if (hr == 0) hr += 12;
  if (hr > 12) hr -= 12;
  return hr;
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	PblTm currentTime;
	get_time(&currentTime);

	if (current_year != currentTime.tm_year) {
		current_year = currentTime.tm_year - 88;
		calculate_digit_positions(current_year);
		for (int i = 0; i < 3; ++i) {
			if (i < digit_used){
				layer_set_hidden(&bml_year[i].layer, false);
				bitmap_layer_set_bitmap(&bml_year[i], &image_numbers_small[digit_pos[i]].bmp);
				layer_set_frame(&bml_year[i].layer, GRect(SCREENW - SMALL_W, (2+i)*SMALL_H, SMALL_W, SMALL_H));
				layer_mark_dirty(&bml_year[i].layer);
			} else {
				layer_set_hidden(&bml_year[i].layer, true);
			}
		}
		layer_set_frame(&image_lbl_yr.layer.layer, GRect(SCREENW - SMALL_W, (2+digit_used)*SMALL_H, SMALL_W, SMALL_H));
		layer_mark_dirty(&image_lbl_yr.layer.layer);
	}

	if(current_month != currentTime.tm_mon) {
		current_month = currentTime.tm_mon;
		calculate_digit_positions(current_month+1);
		for (int i = 0; i < 3; ++i) {
			if (i < digit_used){
				layer_set_hidden(&bml_month[i].layer, false);
				bitmap_layer_set_bitmap(&bml_month[i], &image_numbers_small[digit_pos[i]].bmp);
				layer_set_frame(&bml_month[i].layer, GRect(SCREENW - 2*SMALL_W, i*SMALL_H, SMALL_W, SMALL_H));
				layer_mark_dirty(&bml_month[i].layer);
			} else {
				layer_set_hidden(&bml_month[i].layer, true);
			}
		}
		layer_set_frame(&image_lbl_mt.layer.layer, GRect(SCREENW - 2*SMALL_W, digit_used*SMALL_H, SMALL_W, SMALL_H));
		layer_mark_dirty(&image_lbl_mt.layer.layer);
	}

	if (current_day != currentTime.tm_mday) {
		current_day = currentTime.tm_mday;
		calculate_digit_positions(current_day);
		for (int i = 0; i < 3; ++i) {
			if (i < digit_used){
				layer_set_hidden(&bml_day[i].layer, false);
				bitmap_layer_set_bitmap(&bml_day[i], &image_numbers_small[digit_pos[i]].bmp);
				layer_set_frame(&bml_day[i].layer, GRect(SCREENW - 3*SMALL_W, i*SMALL_H, SMALL_W, SMALL_H));
				layer_mark_dirty(&bml_day[i].layer);
			} else {
				layer_set_hidden(&bml_day[i].layer, true);
			}
		}
		layer_set_frame(&image_lbl_dy.layer.layer, GRect(SCREENW - 3*SMALL_W, digit_used*SMALL_H, SMALL_W, SMALL_H));
		layer_mark_dirty(&image_lbl_dy.layer.layer);
	}
	if (current_wday != currentTime.tm_wday) {
		current_wday = currentTime.tm_wday;
		layer_set_hidden(&bml_weekday.layer, false);
		bitmap_layer_set_bitmap(&bml_weekday, &image_week_days[current_wday].bmp);
		layer_set_frame(&bml_weekday.layer, GRect(SCREENW - 4*SMALL_W, 0, SMALL_W, SMALL_H));
		layer_mark_dirty(&bml_weekday.layer);
	}

	if (current_hour != currentTime.tm_hour) {
		current_hour = currentTime.tm_hour;
		calculate_digit_positions(calculate_12_format(current_hour));

		if (current_hour >= 5 && current_hour < 12) {
			layer_set_hidden(&img_morning.layer.layer, false);
			layer_set_hidden(&img_noon.layer.layer, true);
			layer_set_hidden(&img_night.layer.layer, true);
		} else if (current_hour >= 12 && current_hour < 19) {
			layer_set_hidden(&img_morning.layer.layer, true);
			layer_set_hidden(&img_noon.layer.layer, false);
			layer_set_hidden(&img_night.layer.layer, true);
		} else {
			layer_set_hidden(&img_morning.layer.layer, true);
			layer_set_hidden(&img_noon.layer.layer, true);
			layer_set_hidden(&img_night.layer.layer, false);
		}

		for (int i = 0; i < 3; ++i) {
			if (i < digit_used){
				layer_set_hidden(&bml_hour[i].layer, false);
				bitmap_layer_set_bitmap(&bml_hour[i], &image_numbers_small[digit_pos[i]].bmp);
				layer_set_frame(&bml_hour[i].layer, GRect(SCREENW - 6*SMALL_W, i*SMALL_H, SMALL_W, SMALL_H));
				layer_mark_dirty(&bml_hour[i].layer);
			} else {
				layer_set_hidden(&bml_hour[i].layer, true);
			}
		}
		layer_set_frame(&image_lbl_hr.layer.layer, GRect(SCREENW - 6*SMALL_W, digit_used*SMALL_H, SMALL_W, SMALL_H));
		layer_mark_dirty(&image_lbl_hr.layer.layer);
	}

	if (current_minute != currentTime.tm_min) {
		current_minute = currentTime.tm_min;
		calculate_digit_positions(current_minute);
		for (int i = 0; i < 3; ++i) {
			if (i < digit_used){
				layer_set_hidden(&bml_minute[i].layer, false);
				bitmap_layer_set_bitmap(&bml_minute[i], &image_numbers_small[digit_pos[i]].bmp);
				layer_set_frame(&bml_minute[i].layer, GRect(SCREENW - 7*SMALL_W, i*SMALL_H, SMALL_W, SMALL_H));
				layer_mark_dirty(&bml_minute[i].layer);
			} else {
				layer_set_hidden(&bml_minute[i].layer, true);
			}
		}
		layer_set_frame(&image_lbl_mn.layer.layer, GRect(SCREENW - 7*SMALL_W, digit_used*SMALL_H, SMALL_W, SMALL_H));
		layer_mark_dirty(&image_lbl_mn.layer.layer);
	}
	
}

void handle_init(AppContextRef ctx) {
	(void)ctx;

	window_init(&window, "Nanji");
	// window_set_background_color(&window, GColorBlack);
	window_stack_push(&window, true /* Animated */);

	resource_init_current_app(&NANJI_DESUKA);

	/* Label for year */
	bmp_init_container(RESOURCE_ID_IMG_YEAR1, &image_lbl_yr1);
	layer_set_frame(&image_lbl_yr1.layer.layer, GRect(SCREENW - SMALL_W, 0, SMALL_W, SMALL_H));
	layer_add_child(&window.layer, &image_lbl_yr1.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_YEAR2, &image_lbl_yr2);
	layer_set_frame(&image_lbl_yr2.layer.layer, GRect(SCREENW - SMALL_W, SMALL_H, SMALL_W, SMALL_H));
	layer_add_child(&window.layer, &image_lbl_yr2.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_FUJI, &image_bg);
	layer_set_frame(&image_bg.layer.layer, GRect(SCREENW - LANDS_W, SCREENH - LANDS_H, LANDS_W, LANDS_H));
	layer_add_child(&window.layer, &image_bg.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_WEEKD1, &image_lbl_wd1);
	layer_set_frame(&image_lbl_wd1.layer.layer, GRect(SCREENW - 4*SMALL_W, 1*SMALL_H, SMALL_W, SMALL_H));
	layer_add_child(&window.layer, &image_lbl_wd1.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_WEEKD2, &image_lbl_wd2);
	layer_set_frame(&image_lbl_wd2.layer.layer, GRect(SCREENW - 4*SMALL_W, 2*SMALL_H, SMALL_W, SMALL_H));
	layer_add_child(&window.layer, &image_lbl_wd2.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_YEARS, &image_lbl_yr);
	layer_add_child(&window.layer, &image_lbl_yr.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_MONTHS, &image_lbl_mt);
	layer_add_child(&window.layer, &image_lbl_mt.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_DAYS, &image_lbl_dy);
	layer_add_child(&window.layer, &image_lbl_dy.layer.layer);

	// bmp_init_container(RESOURCE_ID_IMG_WMON, &image_lbl_wd);
	// layer_add_child(&window.layer, &image_lbl_wd.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_HOURS, &image_lbl_hr);
	layer_add_child(&window.layer, &image_lbl_hr.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_MINUTES, &image_lbl_mn);
	layer_add_child(&window.layer, &image_lbl_mn.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_MORNING, &img_morning);
	bmp_init_container(RESOURCE_ID_IMG_NOON, &img_noon);
	bmp_init_container(RESOURCE_ID_IMG_NIGHT, &img_night);
	layer_set_frame(&img_morning.layer.layer, GRect(SCREENW - 5*SMALL_W, digit_used*SMALL_H, SMALL_W, SMALL_H));
	layer_set_frame(&img_noon.layer.layer, GRect(SCREENW - 5*SMALL_W, digit_used*SMALL_H, SMALL_W, SMALL_H));
	layer_set_frame(&img_night.layer.layer, GRect(SCREENW - 5*SMALL_W, digit_used*SMALL_H, SMALL_W, SMALL_H));
	layer_add_child(&window.layer, &img_morning.layer.layer);
	layer_add_child(&window.layer, &img_noon.layer.layer);
	layer_add_child(&window.layer, &img_night.layer.layer);

	bmp_init_container(RESOURCE_ID_IMG_S0, &image_numbers_small[0]);
	bmp_init_container(RESOURCE_ID_IMG_S1, &image_numbers_small[1]);
	bmp_init_container(RESOURCE_ID_IMG_S2, &image_numbers_small[2]);
	bmp_init_container(RESOURCE_ID_IMG_S3, &image_numbers_small[3]);
	bmp_init_container(RESOURCE_ID_IMG_S4, &image_numbers_small[4]);
	bmp_init_container(RESOURCE_ID_IMG_S5, &image_numbers_small[5]);
	bmp_init_container(RESOURCE_ID_IMG_S6, &image_numbers_small[6]);
	bmp_init_container(RESOURCE_ID_IMG_S7, &image_numbers_small[7]);
	bmp_init_container(RESOURCE_ID_IMG_S8, &image_numbers_small[8]);
	bmp_init_container(RESOURCE_ID_IMG_S9, &image_numbers_small[9]);
	bmp_init_container(RESOURCE_ID_IMG_S10, &image_numbers_small[10]);

	// bmp_init_container(RESOURCE_ID_IMG_L0, &image_numbers_large[0]);
	// bmp_init_container(RESOURCE_ID_IMG_L1, &image_numbers_large[1]);
	// bmp_init_container(RESOURCE_ID_IMG_L2, &image_numbers_large[2]);
	// bmp_init_container(RESOURCE_ID_IMG_L3, &image_numbers_large[3]);
	// bmp_init_container(RESOURCE_ID_IMG_L4, &image_numbers_large[4]);
	// bmp_init_container(RESOURCE_ID_IMG_L5, &image_numbers_large[5]);
	// bmp_init_container(RESOURCE_ID_IMG_L6, &image_numbers_large[6]);
	// bmp_init_container(RESOURCE_ID_IMG_L7, &image_numbers_large[7]);
	// bmp_init_container(RESOURCE_ID_IMG_L8, &image_numbers_large[8]);
	// bmp_init_container(RESOURCE_ID_IMG_L9, &image_numbers_large[9]);
	// bmp_init_container(RESOURCE_ID_IMG_L10, &image_numbers_large[10]);

	bmp_init_container(RESOURCE_ID_IMG_WSUN, &image_week_days[0]);
	bmp_init_container(RESOURCE_ID_IMG_WMON, &image_week_days[1]);
	bmp_init_container(RESOURCE_ID_IMG_WTUE, &image_week_days[2]);
	bmp_init_container(RESOURCE_ID_IMG_WWED, &image_week_days[3]);
	bmp_init_container(RESOURCE_ID_IMG_WTHU, &image_week_days[4]);
	bmp_init_container(RESOURCE_ID_IMG_WFRI, &image_week_days[5]);
	bmp_init_container(RESOURCE_ID_IMG_WSAT, &image_week_days[6]);

	for (int i = 0; i < 3; ++i) {
		bitmap_layer_init(&bml_year[i], GRect(0, 0, SMALL_W, SMALL_W));
		layer_add_child(&window.layer, &bml_year[i].layer);

		bitmap_layer_init(&bml_month[i], GRect(0, 0, SMALL_W, SMALL_W));
		layer_add_child(&window.layer, &bml_month[i].layer);

		bitmap_layer_init(&bml_day[i], GRect(0, 0, SMALL_W, SMALL_W));
		layer_add_child(&window.layer, &bml_day[i].layer);

		bitmap_layer_init(&bml_hour[i], GRect(0, 0, SMALL_W, SMALL_W));
		layer_add_child(&window.layer, &bml_hour[i].layer);

		bitmap_layer_init(&bml_minute[i], GRect(0, 0, SMALL_W, SMALL_W));
		layer_add_child(&window.layer, &bml_minute[i].layer);
	}
	bitmap_layer_init(&bml_weekday, GRect(0, 0, SMALL_W, SMALL_W));
	layer_add_child(&window.layer, &bml_weekday.layer);

	handle_minute_tick(ctx, NULL);
}

void handle_deinit(AppContextRef ctx) {
	(void) ctx;

	bmp_deinit_container(&img_morning);
	bmp_deinit_container(&img_noon);
	bmp_deinit_container(&img_night);

	bmp_deinit_container(&image_bg);
	bmp_deinit_container(&image_lbl_yr1);
	bmp_deinit_container(&image_lbl_yr2);

	bmp_deinit_container(&image_lbl_yr);
	bmp_deinit_container(&image_lbl_mt);
	bmp_deinit_container(&image_lbl_dy);
	bmp_deinit_container(&image_lbl_hr);
	bmp_deinit_container(&image_lbl_mn);
	bmp_deinit_container(&image_lbl_wd1);
	bmp_deinit_container(&image_lbl_wd2);

	for (int i = 0; i < 11; ++i) {
		bmp_deinit_container(&image_numbers_small[i]);
		// bmp_deinit_container(&image_numbers_large[i]);
		if (i < 7){
			bmp_deinit_container(&image_week_days[i]);
		}
	}
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	app_event_loop(params, &handlers);
}