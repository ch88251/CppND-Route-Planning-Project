FROM cfhayes/cpp-io2d:1.0.0

RUN adduser develop
USER develop

WORKDIR /home/develop

COPY . .
