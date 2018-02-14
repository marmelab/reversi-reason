FROM node:6

RUN npm install -g bs-platform create-react-app --unsafe-perm

RUN mkdir /app
WORKDIR /app
ADD . /app/
