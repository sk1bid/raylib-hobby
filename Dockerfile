FROM --platform=arm64 emscripten/emsdk:4.0.12 AS builder

ARG Game=Doodler

WORKDIR /workspace

COPY scripts/build_web.sh ./build_web.sh
RUN chmod +x build_web.sh

COPY src/ ./src/
COPY raylib/ ./raylib/

RUN ./build_web.sh release "${GAME}"

FROM nginx:stable-alpine

RUN rm -rf /usr/share/nginx/html/*

ARG GAME
COPY --from=builder /workspace/site-root/"${GAME}}"/ /usr/share/nginx/html/

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]