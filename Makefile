all: json.h ../hasak-js/src/hasakProperties110.js

json.h: config.h scripts/nrpn.tcl
	./scripts/nrpn.tcl output min-json-in-c > json.h

hasakProperties110.js: config.h scripts/nrpn.tcl
	./scripts/nrpn.tcl > hasakProperties110.js

../hasak-js/src/hasakProperties110.js: hasakProperties110.js
	cp hasakProperties110.js ../hasak-js/src/hasakProperties110.js
