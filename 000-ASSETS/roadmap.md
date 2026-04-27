# Phase 1 — Pi setup & networking foundations

**Time**: 1-2 days

## Resources

- The Raspberry Pi official [documentation](https://www.raspberrypi.com/documentation/) for the Pi Zero specifically. Focus on the getting started section and the networking section. This covers flashing the OS, enabling SSH, and configuring WiFi headlessly (no monitor needed) via `wpa_supplicant.conf` and an empty ssh file on the boot partition -- which is the right way to set up a Pi Zero since it has no HDMI port.

- For static IP assignment, read `man 5 dhcpcd.conf` on the Pi itself once it's running. Raspberry Pi OS uses `dhcpcd` by default. Setting a static IP is four lines in that config file.
- `man 7 ip` and `man 7 tcp`, but read these early, before you write any code. Understanding what an IP address, port, and TCP connection actually are at the kernel level grounds everything that comes after.

- For cross-compilation specifically, the [GCC manual](https://gcc.gnu.org/onlinedocs/gcc/) covers the `--host`, `--build`, and `--target` flags. You don't need to read much here -- mostly you just need to know that `arm-linux-gnueabihf-gcc` exists and how to invoke it.

## Roadblocks

- The Pi Zero only has one micro-USB port for data (the other is power only). That means no ethernet adapter without a USB hub. You're entirely dependent on WiFi, and the onboard WiFi on the Zero W is adequate but not robust. If your router is far away or congested, SSH sessions can drop unpredictably. Setting up SSH key authentication early (so you're not typing passwords over a flaky connection) saves a lot of frustration.

- Headless setup has one common failure mode -- a typo in `wpa_supplicant.conf` means the Pi boots but never connects to WiFi, and you have no way to know without plugging it into a monitor. Triple-check the SSID and password, and make sure the country code is set correctly otherwise the WiFi radio won't initialise at all.

- SD card quality matters more than people expect. A cheap or slow SD card makes the whole system feel sluggish and fails sooner. A name-brand A1 or A2-rated card (Samsung, SanDisk) is worth the small extra cost.

- Cross-compilation can get complicated if your server links against any libraries, because those libraries also need ARM versions available on your build machine. For now, if you're only using stdlib and POSIX, it's a non-issue. Just be aware this is where cross-compilation setups typically get messy as projects grow. 

## Valuable learning

- Setting up a Pi headlessly forces you to understand how Linux boots, how WiFi is configured at the OS level, and how SSH actually works -- things most developers take for granted because they've always had a GUI. It's a good grounding exercise.

- Understanding dhcpcd and static IP assignment teaches you the difference between DHCP (the router handing out addresses dynamically) and a static configuration, and why a server needs the latter -- if the IP changes, your client can no longer find it.

- The cross-compilation setup, even though it's just installing one package and using a different compiler name, is a good moment to read about what a toolchain actually is: the compiler, assembler, linker, and C runtime all targeting a specific architecture. `man gcc` and the `--target` flag are worth at least a skim for this reason. 

> [!NOTE] 
> Get comfortable with `netcat` and Wireshark. 
> `netcat` lets you simulate both sides of a TCP connection manually, and Wireshark lets you see exactly what bytes are on the wire.

# Phase 2 — HTTP server core (from scratch in C)

**Time**: 5-7 days

## Resources

- [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112) for the HTTP message format. Sections 2–4 cover everything.

- [RFC 9110](https://www.rfc-editor.org/rfc/rfc9110) for HTTP semantics. Useful for understanding status codes, method definitions, and what a spec-compliant response actually looks like

- [Beej's Guide to Network Programming chapters 1–6](https://beej.us/guide/bgnet/). The authoritative learning resource for BSD sockets in C.

- The man pages for every syscall you'll use. These are already on your machine:
```bash
man 2 socket
man 2 bind
man 2 listen
man 2 accept
man 2 read
man 2 write
man 2 close
man 7 ip
man 7 tcp
```

- The [POSIX spec](https://pubs.opengroup.org/onlinepubs/9699919799/) for `read()` and `write()`. The critical detail is that both calls can return fewer bytes than you requested -- this is not an error, and handling it correctly is essential for a socket server

- [cppreference](https://en.cppreference.com/w/c) for C standard library string functions. You'll be living in `strstr`, `memchr`, `strtol`, and `snprintf` for the parsing work. 

## Roadblocks

- Reading from a socket isn't like reading a file. Data arrives in chunks and a single `read()` call may give you half an HTTP request, or two requests glued together. You need to keep reading and accumulating bytes until you've seen `\r\n\r\n`, then decide if there's a body based on `Content-Length`.

- Error handling is verbose. Almost every syscall returns -1 on failure and sets `errno`. You need to check every `read`, `write`, `close`, and `malloc` return value. Skipping these checks leads to silent misbehaviour that is hard to debug later.

- The sequential blocking loop means only one client is handled at a time. For a single client polling every few minutes this is completely fine, but be aware of the limitation. If you ever want concurrency the next step would be `fork()` per connection or `select()/poll()`.

## Valuable learning

- Implementing the incremental `read()` loop until `\r\n\r\n` is one of the most instructive exercises in networked systems programming. It makes the stream nature of TCP concrete in a way that reading about it never does.

- Writing the response manually -- composing the status line, headers, blank line, and body as a single buffer -- shows you exactly what HTTP libraries are doing under the hood and why `Content-Length` matters.

- Understanding why `SO_REUSEADDR` is necessary (the `TIME_WAIT` state after a connection closes) gives you a real glimpse into how TCP connection teardown works at the kernel level.

# Phase 3 — Image generation & serving

**Time**: 2-3 days

## Resources

- The BMP file format spec. [Microsoft's official documentation](https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-storage) is the primary source. Focus on the `BITMAPFILEHEADER` and `BITMAPINFOHEADER` structs -- that's the entire format for a simple uncompressed BMP.

- If you want PNG instead of BMP (smaller, better for e-ink), read the [PNG spec](https://www.w3.org/TR/png/). Fair warning -- PNG is significantly more complex because of its compression (DEFLATE). BMP first, PNG later if you want the challenge.

- For actually drawing text onto your image, look into bitmap fonts at [The BDF font format spec](https://adobe-type-tools.github.io/font-tech-notes/pdfs/5005.BDF_Spec.pdf.). A simpler approach is embedding a hardcoded bitmap font as a C array -- search for "font8x8" on GitHub for a public domain example you can drop straight in.

## Roadblocks

- BMP row padding. BMP requires each pixel row to be padded to a multiple of 4 bytes.

- Serving binary data correctly over HTTP. You need to set `Content-Type: image/bmp` and `Content-Length` to the exact byte count of the image, and make sure your `write()` loop sends every byte. A partial write is a valid return from `write()` on a socket -- you must loop until all bytes are sent.

## Valuable learning

- Writing the BMP header by hand teaches you a lot about struct layout, endianness, and how binary file formats actually work. This is genuinely useful knowledge that transfers everywhere

- The `Content-Length` + binary `write()` loop is a microcosm of a real problem in systems programming -- ensuring complete transmission over a stream that may deliver bytes in pieces.

# Phase 4 — Dashboard data & layout

**Time**: 4-5 days

## Resources

- For making outbound HTTP requests (to fetch data) from C, read Beej's guide chapter 5 again -- you already have the socket primitives, you're just using them as a client rather than a server. You'll open a socket, `connect()` to a remote host, send an HTTP GET request as a string, and read the response.

- DNS resolution: `man 3 getaddrinfo`. This is the modern POSIX way to resolve hostnames to IP addresses. It replaces the older `gethostbyname` -- use `getaddrinfo` exclusively.

- For JSON parsing, read the [JSON spec](https://www.json.org). It's one page. Then look at [jsmn](https://github.com/zserge/jsmn) -- it's a single-header, zero-allocation JSON tokeniser in ~400 lines of C. Reading its source is itself a good learning exercise.

- For the layout engine, there's no spec to read -- you're designing it yourself. Think of it as a 2D array of pixels and a set of functions that draw rectangles, lines, and characters into it.

## Roadblocks

- TLS. Most APIs today are HTTPS only. Raw sockets speak plain HTTP -- adding TLS requires either linking against OpenSSL/mbedTLS, or routing your outbound requests through a local proxy like `stunnel.` This is the single biggest surprise for people building HTTP clients from scratch. Decide early whether you'll use a library for this one piece or work around it (some APIs still have HTTP endpoints, or you can proxy through your own server).

- `getaddrinfo` returns a linked list of results and requires cleanup with `freeaddrinfo`. Easy to forget and leak.

- The layout engine sounds simple but text wrapping, alignment, and handling variable-length data add up. Keep it rigid and column-based early on -- don't try to build a general-purpose layout system.

## Valuable learning

- Writing an HTTP client using raw sockets makes the client-server symmetry click in a way that using libcurl never would. You've built both sides of the conversation.

- `getaddrinfo` is the right way to understand how DNS fits into the socket lifecycle -- it's the missing link between a hostname and the `connect()` call.

- Building even a trivial layout engine teaches you how GUIs work at their core -- everything is just rectangles and pixel buffers.

# Phase 5 — Hardening & go live

**Time**: 1-2 days

## Resources

- `man 7 daemon` -- the Linux man page describing what it means to properly daemonize a process. Covers forking, detaching from the terminal, closing file descriptors, and signal handling.

- systemd unit files: `man 5 systemd.service`. Focus on `ExecStart`, `Restart=always`, `RestartSec`, and `WantedBy`. The [official systemd documentation](https://systemd.io) is also good.

- Signal handling: `man 2 signal` and `man 2 sigaction`. You want to catch `SIGTERM` and `SIGINT` to shut down cleanly -- closing the socket and flushing any state before exit.

- `man 3 syslog` for logging. Writing to syslog integrates naturally with systemd's journal, so `journalctl -u yourservice` just works.

## Roadblocks

- The Pi Zero's SD card is its weakest point. Frequent writes (logs, cached data) will wear it out over months/years. Use `tmpfs` for anything ephemeral and keep writes to a minimum. This is a real concern for always-on Pi projects.

- Power loss mid-write can corrupt the filesystem. Don't write critical state to disk without an atomic write pattern (write to a temp file, then `rename()` -- which is atomic on Linux).

## Valuable learning

- Daemonizing a process properly teaches you a lot about how Unix process groups, sessions, and terminal attachment actually work -- concepts that are invisible when you just run things in a shell.

- Signal handling is one of those topics that every C developer should understand but many skip. Implementing a clean shutdown handler is a small investment with a big conceptual payoff.

- Writing to syslog and thinking about log levels (`LOG_INFO`, `LOG_ERR`, `LOG_DEBUG`) forces you to think about observability -- how do you know your server is healthy from the outside?


