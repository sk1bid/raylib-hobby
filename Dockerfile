FROM emscripten/emsdk:4.0.12 AS builder


ARG RAYLIB_VERSION=4.6.0
ARG GAME=Doodler

WORKDIR /workspace

RUN apt-get update && apt-get install -y --no-install-recommends \
      wget ca-certificates build-essential cmake ninja-build && \
    rm -rf /var/lib/apt/lists/*


RUN wget -qO raylib.tar.gz \
      https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz && \
    mkdir raylib && \
    tar --strip-components=1 -xzf raylib.tar.gz -C raylib && \
    rm raylib.tar.gz

WORKDIR /workspace/raylib
RUN emcmake cmake -G Ninja \
      -B build -S . \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_EXAMPLES=OFF \
      -DBUILD_TESTS=OFF \
      -DPLATFORM=Web && \
    cmake --build build && \
    cp ./libraylib.a src/libraylib.web.a


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

CMD ["nginx","-g","daemon off;"]
