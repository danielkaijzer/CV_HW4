Author: Daniel A. Kaijzer
CSCI 49369 - Computational Vision
HW 4


FORMULA FOR S2:

Given the brightest point (x,y) on sphere's projected image, centroid (xbar, ybar) and radius r:

	light source direction = normal vector = (dx/length, dy/length, z/length)
	light source vector = normal vector * I
	
	where:
		# dx and dy are vectors from center to point x,y
		dx = x - xbar
		dy = y - ybar

		# z is calculated by rearranging sphere equation
		# since we're doing orthographic projection we can treat z as constant
		z = root(r^2 - dx^2 - dy^2)

		# use length to normalize to unit length
		length = root(dx^2 + dy^2 + z^2)

		I is intensity of brightest pixel