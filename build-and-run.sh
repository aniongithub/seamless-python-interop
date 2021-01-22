docker build -t seamless-python-interop-devcontainer -f devcontainer.Dockerfile . &&\
docker build -t seamless-python-interop . &&\
docker run -it seamless-python-interop