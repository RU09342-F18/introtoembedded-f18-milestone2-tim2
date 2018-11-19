
BEGIN {
    NAME=ARGV[2]
    ARGV[2] = ""
    print "// Thermistor table for " NAME
    print ""
    print "#ifndef " toupper(NAME)
    print "#define " toupper(NAME)
    print ""
    print "double thermistor_table[] = {"
    printf("    ")
}

NR > 3 {
    diff = $1 - last
}

NR > 2 {
    printf("%f, ", $5)
    last = $1
}

(NR-2) % 8 == 0 {
    print ""
    printf("    ")
}

END {
    print ""
    print "}"
    print ""
    print "#define THERMISTOR_STEP_DELTA " diff
    print "#define THERMISTOR_STEP_MAX " NR
    print ""
    print "#endif"
}

