FROM --platform=arm64 emscripten/emsdk:4.0.12 AS builder

ARG GAME=Doodler

WORKDIR /workspace

COPY raylib/src ./raylib/src


COPY scripts/ ./scripts/
COPY src/ ./src/

ENV RAYLIB_WEB=/workspace/raylib/src \
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