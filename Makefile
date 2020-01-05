all:

ci:
	rm -rf build
	docker build -t loki-cpp-ci .
