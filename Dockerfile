# Use a base image with the desired operating system and C++ compiler
FROM alpine:3.17.0

# Set the working directory inside the container
WORKDIR /sg

# Only copy build script so that these layers aren't disturbed as often
COPY build_container.sh build_container.sh

RUN chmod +x build_container.sh && \
    ./build_container.sh

# Copy the C++ project files to the working directory
COPY . .

# Build the C++ project using CMake
RUN mkdir Build && \
    cd Build && \
    cmake .. && \
    make

EXPOSE 25565
# Set the entry point for the container
CMD ["./Build/spruce_goose", "25565"]