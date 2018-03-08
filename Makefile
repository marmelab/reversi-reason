.PHONY: help install build start test deploy-demo

USER_ID = $(shell id -u)
GROUP_ID = $(shell id -g)

export UID = $(USER_ID)
export GID = $(GROUP_ID)

BIN = docker run -i -t --rm \
	--user "${UID}:${GID}" \
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

connect: ## Connect to the container
	$(BIN) /bin/bash

deploy-demo: build ## Deploy the demo at http://marmelab.com/reversi-reason/
	cd build && \
	git init && \
	git add . && \
	git commit -m "Update demo" && \
	git remote add origin git@github.com:marmelab/reversi-reason.git && \
	git push --force origin master:gh-pages
	rm -Rf build
