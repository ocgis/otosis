
BEGIN {
	OFS = "";
	num = 0;
}

/^bool	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "#define Opt_", $2, "\t(Options[", num "].value.i)";
	++num;
	next;
}

/^int	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "#define Opt_", $2, "\t(Options[", num "].value.i)";
	++num;
	next;
}

/^str	/ {
	if (letters[$3] && $3 != "-") {
		print "Warning: option letter ", $3, " used for ", \
			  letters[$3], " and ", $2 "!";
	}
	letters[$3] = $2;
	print "#define Opt_", $2, "\t(Options[", num "].value.s)";
	++num;
	next;
}

