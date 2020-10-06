/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "asio.hpp"

#include "Logger.h"

#include <chrono>
#include <mutex>

namespace tvheadend
{
namespace utilities
{

class TCPSocket
{
public:
  TCPSocket() = delete;
  TCPSocket(const std::string& host, uint16_t port) : m_host(host), m_port(port) {}

  virtual ~TCPSocket() { Close(); }

  bool Open(uint64_t iTimeoutMs)
  {
    auto endpoints = asio::ip::tcp::resolver(m_context).resolve(m_host, std::to_string(m_port));

    std::error_code error;
    asio::async_connect(
        m_socket, endpoints,
        [&](const std::error_code& result_error, const asio::ip::tcp::endpoint&) {
          error = result_error;
        });

    RunFor(iTimeoutMs);

    if (error && error != asio::error::operation_aborted)
      Logger::Log(LogLevel::LEVEL_ERROR, "failed to open socket (%s)", error.message().c_str());

    return !error;
  }

  void Close() { m_socket.close(); }

  int64_t Read(void* data, size_t len, uint64_t iTimeoutMs = 0)
  {
    std::error_code error;
    int64_t n = -1;
    asio::async_read(
        m_socket, asio::buffer(data, len),
        asio::bind_executor(m_read, [&](const std::error_code& result_error, std::size_t result_n) {
          n = result_n;
          error = result_error;
        }));

    RunFor(iTimeoutMs);

    if (error && error != asio::error::operation_aborted)
      Logger::Log(LogLevel::LEVEL_ERROR, "failed to read from socket (%s)",
                  error.message().c_str());

    return !error ? n : -1;
  }

  int64_t Write(void* data, size_t len, uint64_t iTimeoutMs)
  {
    std::error_code error;
    int64_t n = -1;
    asio::async_write(m_socket, asio::buffer(data, len),
                      asio::bind_executor(
                          m_write, [&](const std::error_code& result_error, std::size_t result_n) {
                            n = result_n;
                            error = result_error;
                          }));

    RunFor(iTimeoutMs);

    if (error && error != asio::error::operation_aborted)
      Logger::Log(LogLevel::LEVEL_ERROR, "failed to write to socket (%s)",
                  error.message().c_str());

    return !error ? n : -1;
  }

private:
  void RunFor(uint64_t iTimeoutMs)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_context.restart();

    if (iTimeoutMs == 0)
    {
      // block until all work has finished
      m_context.run();
    }
    else
    {
      // block until all work has finished our time is up
      m_context.run_for(std::chrono::milliseconds(iTimeoutMs));
    }
  }

  std::string m_host;
  uint16_t m_port = 0;

  std::mutex m_mutex;

  asio::io_context m_context;
  asio::ip::tcp::socket m_socket{m_context};
  asio::io_context::strand m_read{m_context};
  asio::io_context::strand m_write{m_context};
};

} // namespace utilities
} // namespace tvheadend
