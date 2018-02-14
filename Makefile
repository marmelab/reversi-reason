.PHONY: help install build start test

BIN = docker run -i -t --rm \
	-v "${PWD}:/app" \
	-p "3000:3000" \
	reversi-reason

help: ## Display available commands
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

install: ## Install dependencies
	docker build --tag=reversi-reason .
	$(BIN) yarn install

build: ## Build project
	$(BIN) yarn build

start: ## Start project
	$(BIN) yarn start

test: ## Test Project
	$(BIN) yarn test
