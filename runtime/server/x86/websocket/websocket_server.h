// Copyright (c) 2020 Mobvoi Inc (Binbin Zhang)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef WEBSOCKET_WEBSOCKET_SERVER_H_
#define WEBSOCKET_WEBSOCKET_SERVER_H_

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "boost/asio/connect.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/websocket.hpp"
#include "glog/logging.h"

#include "decoder/symbol_table.h"
#include "decoder/torch_asr_decoder.h"
#include "decoder/torch_asr_model.h"
#include "frontend/feature_pipeline.h"

namespace wenet {

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace http = beast::http;            // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace asio = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;        // from <boost/asio/ip/tcp.hpp>

class ConnectionHandler {
 public:
  ConnectionHandler(tcp::socket&& socket,
                    std::shared_ptr<FeaturePipelineConfig> feature_config,
                    std::shared_ptr<DecodeOptions> decode_config,
                    std::shared_ptr<SymbolTable> symbol_table,
                    std::shared_ptr<TorchAsrModel> model)
      : ws_(std::move(socket)),
        feature_config_(feature_config),
        decode_config_(decode_config),
        symbol_table_(symbol_table),
        model_(model) {}

  ConnectionHandler(ConnectionHandler&& other)
      : ws_(std::move(other.ws_)),
        feature_config_(other.feature_config_),
        decode_config_(other.decode_config_),
        symbol_table_(other.symbol_table_),
        model_(other.model_) {}

  void operator()();

 private:
  websocket::stream<tcp::socket> ws_;
  std::shared_ptr<FeaturePipelineConfig> feature_config_;
  std::shared_ptr<DecodeOptions> decode_config_;
  std::shared_ptr<SymbolTable> symbol_table_;
  std::shared_ptr<TorchAsrModel> model_;
};

class WebSocketServer {
 public:
  WebSocketServer(int port,
                  std::shared_ptr<FeaturePipelineConfig> feature_config,
                  std::shared_ptr<DecodeOptions> decode_config,
                  std::shared_ptr<SymbolTable> symbol_table,
                  std::shared_ptr<TorchAsrModel> model)
      : port_(port),
        feature_config_(feature_config),
        decode_config_(decode_config),
        symbol_table_(symbol_table),
        model_(model) {}

  void Start();

 private:
  int port_;
  // The io_context is required for all I/O
  asio::io_context ioc_{1};
  std::shared_ptr<FeaturePipelineConfig> feature_config_;
  std::shared_ptr<DecodeOptions> decode_config_;
  std::shared_ptr<SymbolTable> symbol_table_;
  std::shared_ptr<TorchAsrModel> model_;
};

}  // namespace wenet

#endif  // WEBSOCKET_WEBSOCKET_SERVER_H_
