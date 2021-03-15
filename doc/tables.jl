# generate ramps for hasak
# ramps are 257 point arrays which are interpolated
cosine_series1(size, k, a0,  a1) = 
    a0 - a1 * cos((2π * k) / (size-1))
cosine_series2( size, k, a0, a1, a2) = 
    cosine_series1(size, k, a0, a1) + a2 * cos((2 * 2π * k) / (size-1))
cosine_series3(size, k, a0, a1, a2, a3) = 
    cosine_series2(size, k, a0, a1, a2) - a3 * cos((3 * 2π * k) / (size-1))
# hann ramp element
hann(k, size) = 
    Integer(round(32767 * cosine_series1(size, k, 0.50, 0.50)))
# blackman harris ramp element
blackman_harris(k, size) = 
    Integer(round(32767 * cosine_series3(size, k, 0.3587500, 0.4882900, 0.1412800, 0.0116800)))
# linear ramp element
triangular(k, size) =
    Integer(round(32767 * (1.0 - abs( (k - ((size-1)/2.0)) / ((size-1)/2.0) ))))
# sine table element
sine(k, size) = 
    Integer(round(32767 * sin(2π * k / size)))

# format tables
using Printf
format_table(t) = join([@sprintf("%s%6d", i>1 && (i%8)==1 ? "\n" : "", t[i]) for i in 1:length(t)], ",")
@printf("const int16_t hann_ramp[257] = {\n%s\n};\n", format_table([hann(k,512) for k in 0:256]))
@printf("const int16_t blackman_harris_ramp[257] = {\n%s\n};\n", format_table([blackman_harris(k,512) for k in 0:256]))
@printf("const int16_t linear_ramp[257] = {\n%s\n};\n", format_table([triangular(k,512) for k in 0:256]))
@printf("const int16_t sine_table[257] = {\n%s\n};\n", format_table([sine(k,256) for k in 0:256]))
