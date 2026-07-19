export function createBoundedFlexFoxHeartbeat(baseUrl, purpose = "device operation") {
  const websocketUrlOverride = process.env.FLEXFOX_WEBSOCKET_URL;
  const websocketUrl = new URL(websocketUrlOverride ?? baseUrl);
  websocketUrl.protocol = "ws:";
  if (!websocketUrlOverride) websocketUrl.port = "81";
  websocketUrl.pathname = "/";
  websocketUrl.search = "";
  websocketUrl.hash = "";

  let socket;
  let heartbeatTimer;
  let reconnectTimer;
  let stopped = true;
  let firstConnectionResolve;
  const firstConnection = new Promise((resolvePromise) => {
    firstConnectionResolve = resolvePromise;
  });

  function clearHeartbeatTimer() {
    if (heartbeatTimer) clearInterval(heartbeatTimer);
    heartbeatTimer = undefined;
  }

  function poke() {
    if (socket?.readyState === WebSocket.OPEN) socket.send("!&");
  }

  function connect() {
    if (stopped) return;
    const candidate = new WebSocket(websocketUrl);
    socket = candidate;

    candidate.addEventListener("open", () => {
      if (stopped || socket !== candidate) return;
      firstConnectionResolve();
      poke();
      clearHeartbeatTimer();
      heartbeatTimer = setInterval(poke, 5000);
    });
    candidate.addEventListener("close", () => {
      if (socket !== candidate) return;
      clearHeartbeatTimer();
      socket = undefined;
      if (!stopped) reconnectTimer = setTimeout(connect, 1500);
    });
  }

  return {
    async start() {
      stopped = false;
      connect();
      await Promise.race([
        firstConnection,
        new Promise((_, reject) => {
          setTimeout(
            () => reject(new Error(`could not establish the bounded AVR heartbeat for ${purpose}`)),
            5000,
          );
        }),
      ]);
    },
    poke,
    stop() {
      stopped = true;
      clearHeartbeatTimer();
      if (reconnectTimer) clearTimeout(reconnectTimer);
      reconnectTimer = undefined;
      if (socket && socket.readyState < WebSocket.CLOSING) {
        socket.close(1000, `${purpose} complete`);
      }
      socket = undefined;
    },
  };
}
