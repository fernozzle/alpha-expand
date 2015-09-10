#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include "lodepng.h"

typedef struct {
	int x;
	int y;
} ivec2;

// Utility functions

static inline unsigned max(unsigned a, unsigned b) {
	return (a > b) ? a : b;
}
static inline unsigned get_index(unsigned x, unsigned y, unsigned width) {
	return y * width + x;
}
unsigned highest_power_of_two(unsigned x) {
	unsigned result = 1;
	while (result < x) {
		result *= 2;
	}
	return result / 2;
}
static inline unsigned dist_2(unsigned x, unsigned y, ivec2 o) {
	int dx = o.x - x;
	int dy = o.y - y;
	return (dx * dx) + (dy * dy);
}
static inline bool is_in_bounds(int x, int y, unsigned width,
	unsigned height) {

	return (x >= 0 && x < width && y >= 0 && y < height);
}

// Not utility functions

void init_nearests(ivec2* nearests, unsigned char* pixels, unsigned width,
	unsigned height) {

	for (unsigned y = 0; y < height; y++)
	for (unsigned x = 0; x < width;  x++) {
		unsigned i = get_index(x, y, width);

		if (pixels[i * 4 + 3] != 0) {
			nearests[i] = (ivec2) {x, y};
		} else {
			nearests[i] = (ivec2) {width * 3, height * 3};
		}
	}
}

const unsigned NEIGHBOR_COUNT = 8;
int off_x[] = {-1,  0,  1, -1, 1, -1, 0, 1};
int off_y[] = {-1, -1, -1,  0, 0,  1, 1, 1};

void jump_flood(ivec2* nearests, unsigned width, unsigned height) {
	unsigned max_step = highest_power_of_two(max(width, height));
	for (unsigned step = max_step; step > 0; step /= 2) {
		for (unsigned y = 0; y < height; y++)
		for (unsigned x = 0; x < width;  x++) {
			ivec2* nearest = nearests + get_index(x, y, width);
			unsigned d_2 = dist_2(x, y, *nearest);

			ivec2 new_nearest;
			unsigned new_d_2;
			for (unsigned n = 0; n < NEIGHBOR_COUNT; n++) {
				int nx = x + off_x[n] * step;
				int ny = y + off_y[n] * step;
				if (!is_in_bounds(nx, ny, width, height)) {
					continue;
				}

				new_nearest = nearests[get_index(
					nx,
					ny,
					width
				)];
				new_d_2 = dist_2(x, y, new_nearest);

				if (new_d_2 < d_2) {
					d_2 = new_d_2;
					*nearest = new_nearest;
				}
			}
		}
	}
}

void copy_nearests(ivec2* nearests, unsigned char* pixels, unsigned width,
	unsigned height) {

	unsigned size = width * height;
	for (unsigned i = 0; i < size; i++) {
		ivec2 nearest = nearests[i];
		unsigned nearest_i = get_index(nearest.x, nearest.y,
			width);
		if (nearest_i != i) {
			pixels[i * 4 + 0] = pixels[nearest_i * 4 + 0];
			pixels[i * 4 + 1] = pixels[nearest_i * 4 + 1];
			pixels[i * 4 + 2] = pixels[nearest_i * 4 + 2];
			pixels[i * 4 + 3] = 0;
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("No images specified.\n");
		exit(EXIT_FAILURE);
	}

	unsigned int error;
	for (unsigned argi = 1; argi < argc; argi++) {
		char* path = argv[argi];

		// Load image

		unsigned char* pixels;
		unsigned width;
		unsigned height;

		error = lodepng_decode32_file(&pixels, &width, &height, path);
		if (error) {
			const char* error_text = lodepng_error_text(error);
			printf("Error loading `%s`: %s\n", path, error_text);

			printf("Press ENTER to continue...\n");
			while(getchar() != '\n');

			exit(EXIT_FAILURE);
		}
		printf("Performing on `%s`...\n", path);

		// Initialize 'nearest' map and jump flood

		unsigned size = width * height;
		ivec2* nearests = malloc(size * sizeof(ivec2));

		init_nearests(nearests, pixels, width, height);
		jump_flood(nearests, width, height);

		// Copy colors into invisible pixels and write image

		copy_nearests(nearests, pixels, width, height);
		free(nearests);

		error = lodepng_encode32_file(path, pixels, width, height);
		free(pixels);
		if (error) {
			const char* error_text = lodepng_error_text(error);
			printf("Error saving to `%s`: %s\n", path, error_text);

			printf("Press ENTER to continue...\n");
			while(getchar() != '\n');

			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
