FROM --platform=arm64 emscripten/emsdk:4.0.12 AS builder

ARG GAME=Doodler

WORKDIR /workspace

COPY raylib/ ./raylib/
RUN cd raylib/src \
 && emcmake cmake -B build -S . -DBUILD_SHARED_LIBS=OFF -DPLATFORM=WEB \
 && cmake --build build --config Release \
 && cp raylib/src/build/libraylib.a libraylib.web.a

COPY scripts/ ./scripts/
COPY src/ ./src/

ENV RAYLIB_WEB=/workspace/libraylib.web.a  \
    OUTDIR_ROOT=/workspace/site-root
    
WORKDIR /workspace/scripts

RUN chmod +x build_web.sh && \
    ./build_web.sh "${GAME}"

FROM nginx:stable-alpine

RUN rm -rf /usr/share/nginx/html/*

ARG GAME
COPY --from=builder /workspace/site-root/"${GAME}}"/ /usr/share/nginx/html/

EXPOSE 80

CMD ["nginx", "-g", "daemon"]