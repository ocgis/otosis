
BEGIN {
	OFS = "";
	print "\nOPTION Options[] = {" >"optiondef.c";
	num = 0;
}

/^bool	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "\t{ \"", $2, "\", OPTTYPE_BOOL, '", $3, "', { .i=", $4, " } }," \
          >"optiondef.c";
	print "#define Opt_", $2, "\t(Options[", num "].value.i)" \
		  >"include/optiondef.h";
	++num;
	next;
}

/^int	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "\t{ \"", $2, "\", OPTTYPE_INT, '", $3, "', { .i=", $4, " } }," \
		  >"optiondef.c";
	print "#define Opt_", $2, "\t(Options[", num "].value.i)" \
		  >"include/optiondef.h";
	++num;
	next;
}

/^str	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "\t{ \"", $2, "\", OPTTYPE_STR, '", $3, "', { .s=", $4, " } }," \
		  >"optiondef.c";
	print "#define Opt_", $2, "\t(Options[", num "].value.s)" \
		  >"include/optiondef.h";
	++num;
	next;
}

END {
	print "\t{ NULL, 0, '-', { .i=0 } }" >"optiondef.c";
	print "};\n" >"optiondef.c";
}
