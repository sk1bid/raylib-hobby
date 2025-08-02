FROM emscripten/emsdk:4.0.12 AS builder

# Set name of game folder to compile
ARG GAME=Doodler

WORKDIR /workspace

RUN apt-get update && apt-get install -y --no-install-recommends \
      git build-essential wget ca-certificates \
    && rm -rf /var/lib/apt/lists/*


ENV CMAKE_VERSION=3.27.4
RUN wget -qO /tmp/cmake.tar.gz \
      https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz \
 && tar --strip-components=1 -xzvf /tmp/cmake.tar.gz -C /usr/local \
 && rm /tmp/cmake.tar.gz

# make raylib web
RUN git clone --depth 1 https://github.com/raysan5/raylib.git raylib
WORKDIR /workspace/raylib
RUN emcmake cmake -B build -S . -DBUILD_SHARED_LIBS=OFF -DPLATFORM=WEB \
 && cmake --build build --config Release \
 && cp build/libraylib.a src/libraylib.web.a


WORKDIR /workspace
COPY scripts/ ./scripts/
COPY src/     ./src/

ENV RAYLIB_WEB=/workspace/raylib/src \
    OUTDIR_ROOT=/workspace/site-root

WORKDIR /workspace/scripts
RUN chmod +x build_web.sh && \
    ./build_web.sh "${GAME}"

FROM nginx:stable-alpine

RUN rm -rf /usr/share/nginx/html/*

ARG GAME

COPY --from=builder /workspace/site-root/"${GAME}"/ /usr/share/nginx/html/

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
