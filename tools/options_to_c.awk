
BEGIN {
	OFS = "";
	print "#include <stdio.h> /* for definition of NULL */\n\n";
	print "#include <option.h>\n";
	print "\nOPTION Options[] = {";
	num = 0;
}

/^bool	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "\t{ \"", $2, "\", OPTTYPE_BOOL, '", $3, "', { .i=", $4, " } },";
	++num;
	next;
}

/^int	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "\t{ \"", $2, "\", OPTTYPE_INT, '", $3, "', { .i=", $4, " } },";
	++num;
	next;
}

/^str	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "\t{ \"", $2, "\", OPTTYPE_STR, '", $3, "', { .s=", $4, " } },";
	++num;
	next;
}

END {
	print "\t{ NULL, 0, '-', { .i=0 } }";
	print "};\n";
}
