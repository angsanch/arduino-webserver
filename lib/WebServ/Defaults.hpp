#pragma once

#include "WebServer.hpp"
#include "definitions.hpp"

// Appends index (with a '/' separator if needed) to path, which lives at
// raw()[1] of a buffer of the given size. Returns false if it would not fit.
inline bool appendIndexPath(char *path, size_t size, PGM_P index)
{
	size_t len = strlen(path);
	size_t indexLen = strlen_P(index);
	size_t slash = (len > 0 && path[len - 1] == '/') ? 0 : 1;

	if (len + slash + indexLen + 2 > size)
		return (false);
	if (slash)
		path[len ++] = '/';
	memcpy_P(&path[len], index, indexLen + 1);
	return (true);
}

// Serves dirpath + request path. The request path is the one left in the shared
// buffer by WebServer::clientLanding (after any entrypoint prefix was removed).
// A directory request serves index.htm. GET queues the client for streaming;
// HEAD only sends the header.
template<const char *dirpath>
void callbackSdcardHandle(WebServerHandle &server, WebClient &client, t_http_method method)
{
	GlobalBuffer buff;
	char *full = &buff.raw()[1];
	size_t dirLen = strlen_P(dirpath);
	size_t pathLen = strlen(full);
	const char *ext;
	const char *slash;
	PGM_P index = GLOBAL_FSTRING("index.htm");
	PGM_P type;
	unsigned long size;
	File32 file;
	WebClient *spot;

	if (dirLen + pathLen + 2 > buff.size())
		return (staticResponse(client, 400));
	memmove(&full[dirLen], full, pathLen + 1);
	memcpy_P(full, dirpath, dirLen);

	if (full[dirLen + pathLen - 1] == '/')
		if (!appendIndexPath(full, buff.size(), index))
			return (staticResponse(client, 400));

	if (!server.exists(full))
		return (staticResponse(client, 404));
	server.getFile(file, full);
	if (!file)
		return (staticResponse(client, 500));
	if (file.isDir()) {
		file.close();
		if (!appendIndexPath(full, buff.size(), index))
			return (staticResponse(client, 400));
		if (!server.exists(full))
			return (staticResponse(client, 404));
		server.getFile(file, full);
		if (!file)
			return (staticResponse(client, 500));
		if (file.isDir())
			return (staticResponse(client, 404));
	}
	size = file.size();

	ext = strrchr(full, '.');
	slash = strrchr(full, '/');
	type = nullptr;
	if (ext && (!slash || ext > slash))
		type = content_type.get(ext + 1);
	if (!type)
		type = GLOBAL_FSTRING("text/plain");

	if (method == HTTP_HEAD) {
		client.header().send(200)
			.sendFieldFlash(GLOBAL_FSTRING("Content-Type"), type)
			.sendField(GLOBAL_FSTRING("Content-Length"), size);
		return ;
	}

	spot = server.getClientSpot();
	if (!spot)
		return (staticResponse(client, 429));
	client.setFile(static_cast<File32 &&>(file));
	client.header().send(200)
		.sendFieldFlash(GLOBAL_FSTRING("Content-Type"), type)
		.sendField(GLOBAL_FSTRING("Content-Length"), size);
	*spot = static_cast<WebClient &&>(client);
}

