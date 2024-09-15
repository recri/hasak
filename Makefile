all: json.h ../hasak-js/src/hasakProperties110.js

json.h: config.h scripts/nrpn.tcl
	./scripts/nrpn.tcl output min-json-in-c > json.h

../hasak-js/src/hasakProperties110.js:  config.h scripts/nrpn.tcl
	./scripts/nrpn.tcl > ../hasak-js/src/hasakProperties110.js
